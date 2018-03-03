/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#include <future>
#include <memory>
#include <string>
#include <utility>

#include "support/threading/DetachingOperationalQueue.h"

namespace support {

    namespace {
        constexpr auto DEFAULT_MAX_DISPATCH_TIME = std::chrono::milliseconds(1000);
    }

    DetachingOperationalQueue::DetachingOperationalQueue()
            : DetachingOperationalQueue("", {}, DEFAULT_MAX_DISPATCH_TIME) {
    }

    DetachingOperationalQueue::DetachingOperationalQueue(std::string thread_name)
            : DetachingOperationalQueue(std::move(thread_name), {}, DEFAULT_MAX_DISPATCH_TIME) {
    }

    DetachingOperationalQueue::DetachingOperationalQueue(std::string thread_name, std::function<void(Thread&&)> detach_delegate, std::chrono::milliseconds max_dispatch_time)
            : OperationalQueue(thread_name, false)
            , _state(std::make_shared<State>()) {
        _state->max_dispatch_time = max_dispatch_time;
        _state->detach_delegate = std::move(detach_delegate);
        start_processing(std::bind(&DetachingOperationalQueue::execute_ticket, _state, std::placeholders::_1));

        std::string watchdog_thread_name = thread_name.empty() ? "" : thread_name + "-w";
        _watchdog_thread = Thread(watchdog_thread_name, std::bind(&DetachingOperationalQueue::processing_watchdog_loop, this));
    }

    DetachingOperationalQueue::~DetachingOperationalQueue() {
        stop_processing();

        {
            std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
            _state->stop_watchdog_requested = true;
            _state->sync_condition.notify_all();
        }

        _watchdog_thread.join();
    }

    void DetachingOperationalQueue::execute_ticket(std::shared_ptr<State> state, TicketDescriptorPtr ticket) {
        {
            std::lock_guard<decltype(state->sync_mutex)> lock(state->sync_mutex);
            state->running_tickets.push(std::make_pair(ticket->handle, ticket->completed_future));
            state->sync_condition.notify_all();
        }

        ticket->invocable();

        {
            std::lock_guard<decltype(state->sync_mutex)> lock(state->sync_mutex);
            state->running_tickets.pop();
        }
    }

    void DetachingOperationalQueue::processing_watchdog_loop() {
        while (true) {
            TicketHandle ticket;
            std::shared_future<void> ticket_future;

            {
                std::unique_lock<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
                _state->sync_condition.wait(lock, [this] {
                    return !_state->running_tickets.empty() || _state->stop_watchdog_requested;
                });

                if (_state->stop_watchdog_requested) {
                    break;
                }

                ticket = _state->running_tickets.front().first;
                ticket_future = _state->running_tickets.front().second;
            }

            const auto status = ticket_future.wait_for(_state->max_dispatch_time);
            if (status != std::future_status::ready) {
                Thread ejected;

                {
                    std::lock_guard<decltype(_state->sync_mutex)> lock(_state->sync_mutex);
                    if (_state->running_tickets.front().first == ticket) {
                        ejected = eject_processing_thread();
                    }
                }

                if (ejected.joinable()) {
                    if (_state->detach_delegate) {
                        _state->detach_delegate(std::move(ejected));
                    } else {
                        ejected.detach();
                    }
                }
            }
        }  // end while(true)
    }
}  // namespace support
