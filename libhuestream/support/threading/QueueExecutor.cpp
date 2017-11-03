/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#include <algorithm>
#include <list>
#include <mutex>
#include <utility>

#include "threading/QueueExecutor.h"

#include "threading/OperationalQueue.h"

namespace huesdk_lib {

    struct QueueExecutor::Impl {
        struct TicketInfo {
            OperationalQueue::TicketHandle ticket;
            bool wait_on_cleanup;
        };

        std::list<TicketInfo> tickets;
        std::shared_ptr<OperationalQueue> queue;
        std::mutex sync;
        bool closing;

        explicit Impl(std::shared_ptr<OperationalQueue> i_queue)
                : queue(i_queue)
                , closing(false) {
        }
    };

    QueueExecutor::QueueExecutor(std::shared_ptr<OperationalQueue> queue)
            : _impl(std::make_shared<Impl>(queue)) {
    }

    QueueExecutor::~QueueExecutor() {
        shutdown();
    }

    void QueueExecutor::shutdown() {
        {
            std::lock_guard<decltype(_impl->sync)> lock(_impl->sync);
            _impl->closing = true;
        }

        clear(CleanupPolicy::PreferDiscard);
    }

    void QueueExecutor::execute(std::function<void()> invocable, bool is_cancelable) {
        post(std::move(invocable), is_cancelable ? CleanupPolicy::PreferDiscard : CleanupPolicy::AlwaysExecute, false);
    }

    void QueueExecutor::execute_and_wait(std::function<void()> invocable) {
        post(std::move(invocable), CleanupPolicy::AlwaysExecute, true);
    }

    void QueueExecutor::wait_all() {
        clear(CleanupPolicy::AlwaysExecute);
    }

    void QueueExecutor::cancel_all() {
        clear(CleanupPolicy::PreferDiscard);
    }

    void QueueExecutor::post(std::function<void()> invocable, QueueExecutor::CleanupPolicy policy, bool wait_complete) {
        {
            std::lock_guard<std::mutex> lock(_impl->sync);
            if (_impl->closing) {
                return;
            }
        }

        std::weak_ptr<Impl> weak_impl = _impl;
        auto this_ticket = std::make_shared<OperationalQueue::TicketHandle>();
        auto ticket = _impl->queue->create_ticket([invocable, weak_impl, this_ticket] {
            invocable();

            auto impl = weak_impl.lock();
            if (impl) {
                std::lock_guard<decltype(impl->sync)> lock(impl->sync);
                impl->tickets.remove_if([this_ticket](const Impl::TicketInfo& info){
                    return info.ticket == *this_ticket;
                });
            }
        });

        *this_ticket = ticket;

        bool posted = false;

        do {
            std::lock_guard<std::mutex> lock(_impl->sync);
            if (_impl->closing) {
                break;
            }

            _impl->tickets.push_back(Impl::TicketInfo{ticket, policy == CleanupPolicy::AlwaysExecute});
            _impl->queue->schedule_ticket(ticket);
            posted = true;
        } while (false);

        if (!posted) {
            _impl->queue->discard_ticket(ticket);
        } else if (wait_complete) {
            _impl->queue->wait_ticket(ticket);
        }
    }

    void QueueExecutor::clear(QueueExecutor::CleanupPolicy policy) {
        std::list<Impl::TicketInfo> tickets;
        std::list<OperationalQueue::TicketHandle> closed_tickets;
        std::shared_ptr<OperationalQueue> queue;

        do {
            {
                std::lock_guard<std::mutex> lock(_impl->sync);

                if (!closed_tickets.empty()) {
                    _impl->tickets.remove_if([&closed_tickets](const Impl::TicketInfo& info) {
                        return std::find(std::begin(closed_tickets), std::end(closed_tickets), info.ticket) != std::end(closed_tickets);
                    });
                }

                tickets = _impl->tickets;
                queue = _impl->queue;
            }

            if (!queue) {
                return;
            }

            // reverse order to discard tickets that are not yet posted since we are not blocking queue here
            for (auto it = tickets.rbegin(); it != tickets.rend(); ++it) {
                if (!it->wait_on_cleanup && policy == CleanupPolicy::PreferDiscard) {
                    queue->discard_ticket(it->ticket);
                    closed_tickets.push_back(it->ticket);
                }
            }

            // forward order for execution
            for (auto it = std::begin(tickets); it != std::end(tickets); ++it) {
                if (it->wait_on_cleanup || policy == CleanupPolicy::AlwaysExecute) {
                    queue->wait_ticket(it->ticket);
                    closed_tickets.push_back(it->ticket);
                }
            }

            // pickup everything that may come in during cleanup
        } while (!tickets.empty());
    }

    std::shared_ptr<OperationalQueue> QueueExecutor::queue() const {
        return _impl->queue;
    }

    std::shared_ptr<QueueExecutor> QueueExecutor::global() {
        auto global_queue = global_execution_queue();
        static auto s_global_instance = std::make_shared<QueueExecutor>(global_queue);
        return s_global_instance;
    }

    std::shared_ptr<OperationalQueue> global_execution_queue() {
        static auto s_instance = std::make_shared<OperationalQueue>("glob-execute");
        return s_instance;
    }
}  // namespace huesdk_lib
