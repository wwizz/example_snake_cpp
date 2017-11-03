/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#include <algorithm>
#include <utility>

#include "threading/OperationalQueue.h"

namespace huesdk_lib {

    OperationalQueue::OperationalQueue()
            : OperationalQueue("", true) {
    }

    OperationalQueue::OperationalQueue(std::string thread_name)
            : OperationalQueue(std::move(thread_name), true) {
    }

    OperationalQueue::OperationalQueue(std::string thread_name, bool auto_start)
            : _state(std::make_shared<State>()) {
        _state->thread_name = std::move(thread_name);
        if (auto_start) {
            start_processing();
        }
    }

    OperationalQueue::~OperationalQueue() {
        stop_processing();
    }

    OperationalQueue::TicketHandle OperationalQueue::create_ticket(std::function<void()> invocable) {
        auto descriptor = std::make_shared<TicketDescriptor>();
        descriptor->handle = std::make_shared<_TicketHandle>();
        descriptor->invocable = std::move(invocable);
        descriptor->completed_future = descriptor->completed_promise.get_future().share();

        std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
        if (_state->stop_processing_requested) {
            return TicketHandle();
        }

        _state->registry[descriptor->handle] = descriptor;
        return descriptor->handle;
    }

    void OperationalQueue::schedule_ticket(const OperationalQueue::TicketHandle& ticket) {
        std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
        if (_state->stop_processing_requested) {
            return;
        }

        auto it = _state->registry.find(ticket);
        if (it == _state->registry.end()) {
            return;
        }

        if (it->second->was_scheduled) {
            return;
        }

        it->second->was_scheduled = true;

        _state->processing_queue.push_back(it->second);
        _state->sync_condition.notify_all();
    }

    void OperationalQueue::discard_ticket(const OperationalQueue::TicketHandle& ticket) {
        bool is_currently_running = false;
        TicketDescriptorPtr removed_ticket;

        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);

            auto it = _state->registry.find(ticket);
            if (it == _state->registry.end())
                return;

            auto current_it = std::find_if(std::begin(_state->current_ticket_stack), std::end(_state->current_ticket_stack), [&ticket] (const TicketDescriptorPtr& d) {
                return ticket == d->handle;
            });

            if (current_it != std::end(_state->current_ticket_stack)) {
                is_currently_running = true;
            } else {
                _state->processing_queue.remove_if([&it](const TicketDescriptorPtr& d) {
                    return it->second->handle == d->handle;
                });

                removed_ticket = it->second;
                _state->registry.erase(it);
            }
        }

        if (removed_ticket) {
            removed_ticket->completed_promise.set_value();
        }

        if (is_currently_running) {
            wait_ticket(ticket);
        }
    }

    void OperationalQueue::wait_ticket(const OperationalQueue::TicketHandle& ticket) {
        TicketDescriptorPtr descriptor;

        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
            auto it = _state->registry.find(ticket);
            if (it == _state->registry.end()) {
                return;
            }

            descriptor = it->second;
        }

        bool recursive_wait = false;

        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
            recursive_wait = std::this_thread::get_id() == _state->processing_thread.get_id();
        }

        if (!recursive_wait) {
            descriptor->completed_future.wait();
            return;
        }

        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);

            auto current_it = std::find_if(std::begin(_state->current_ticket_stack), std::end(_state->current_ticket_stack), [&descriptor] (const TicketDescriptorPtr& d) {
                return descriptor->handle == d->handle;
            });

            if (current_it != std::end(_state->current_ticket_stack)) {
                return;
            }
        }

        processing_loop(_state, [this, ticket] () -> bool {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
            return _state->registry.find(ticket) == _state->registry.end();
        });
    }

    void OperationalQueue::processing_loop(std::shared_ptr<State> state, std::function<bool()> recursion_break_condition) {
        while (!recursion_break_condition()) {
            TicketDescriptorPtr current_ticket;

            {
                std::unique_lock<decltype(state->sync_mutex)> lock(state->sync_mutex);
                state->sync_condition.wait(lock, [state] {
                    return (!state->processing_queue.empty()
                            || state->stop_processing_requested
                            || std::this_thread::get_id() != state->processing_thread.get_id());
                });

                if (state->stop_processing_requested || std::this_thread::get_id() != state->processing_thread.get_id()) {
                    break;
                }

                current_ticket = state->processing_queue.front();
                state->current_ticket_stack.push_back(current_ticket);
                state->processing_queue.pop_front();
            }

            if (state->ticket_executor) {
                state->ticket_executor(current_ticket);
            } else {
                current_ticket->invocable();
            }

            bool processing_thread_was_ejected = false;
            {
                std::lock_guard<decltype(state->sync_mutex)> lock(state->sync_mutex);
                processing_thread_was_ejected = std::this_thread::get_id() != state->processing_thread.get_id();

                if (!processing_thread_was_ejected) {
                    state->current_ticket_stack.pop_back();
                    state->registry.erase(current_ticket->handle);
                }
            }

            if (!processing_thread_was_ejected)
                current_ticket->completed_promise.set_value();
        }
    }

    Thread OperationalQueue::eject_processing_thread() {
        Thread prev_processing_thread;
        decltype(_state->current_ticket_stack) ejected_tickets;

        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
            if (_state->stop_processing_requested) {
                return {};
            }

            prev_processing_thread = Thread(_state->thread_name, std::bind(&OperationalQueue::processing_loop, _state, []{return false;}));
            std::swap(_state->processing_thread, prev_processing_thread);

            for (const auto& descriptor : _state->current_ticket_stack) {
                _state->registry.erase(descriptor->handle);
            }

            std::swap(ejected_tickets, _state->current_ticket_stack);

            _state->sync_condition.notify_all();
        }

        for (const auto& descriptor : ejected_tickets) {
            descriptor->completed_promise.set_value();
        }

        return prev_processing_thread;
    }

    bool OperationalQueue::run_recursive_process_tickets_loop(const std::function<bool()>& break_condition) {
        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
            if (std::this_thread::get_id() != _state->processing_thread.get_id()) {
                return false;
            }
        }

        processing_loop(_state, break_condition);
        return true;
    }

    void OperationalQueue::start_processing(std::function<void(OperationalQueue::TicketDescriptorPtr)> ticket_executor) {
        std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
        if (_state->stop_processing_requested || _state->processing_thread.joinable()) {
            return;
        }

        _state->ticket_executor = std::move(ticket_executor);
        _state->processing_thread = Thread(_state->thread_name, std::bind(&OperationalQueue::processing_loop, _state, []{return false;}));
    }

    void OperationalQueue::stop_processing() {
        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
            if (_state->stop_processing_requested) {
                return;
            }

            _state->stop_processing_requested = true;
            _state->sync_condition.notify_all();
        }

        if (_state->processing_thread.joinable())
            _state->processing_thread.join();
    }

    bool OperationalQueue::has_ticket(const OperationalQueue::TicketHandle &ticket) {
        std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);

        auto it = _state->registry.find(ticket);
        return it != _state->registry.end();
    }

    size_t OperationalQueue::ticket_count() {
        std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
        return _state->registry.size();
    }
}  // namespace huesdk_lib