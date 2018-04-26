/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#include <algorithm>
#include <list>
#include <mutex>
#include <utility>

#include "support/threading/QueueExecutor.h"

#include "support/threading/OperationalQueue.h"

using huesdk::Operation;

namespace support {

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

    class QueueExecutorOperation : public huesdk::IOperation {
    public:
        QueueExecutorOperation() = default;
        QueueExecutorOperation(std::weak_ptr<OperationalQueue> queue,
                               std::weak_ptr<support::OperationalQueue::_TicketHandle> ticket_handle,
                               QueueExecutor::OperationType operation_type)
            : _queue{queue}
            , _ticket_handle{ticket_handle}
            , _operation_type{operation_type} {}

        void wait() override {
            auto queue = _queue.lock();
            auto ticket_handle = _ticket_handle.lock();
            if (queue && ticket_handle) {
                queue->wait_ticket(ticket_handle);
            }
        }

        bool is_cancelable() const override {
            return QueueExecutor::OperationType::CANCELABLE == _operation_type;
        }

        void cancel() override {
            auto queue = _queue.lock();
            auto ticket_handle = _ticket_handle.lock();
            if (queue && ticket_handle) {
                queue->discard_ticket(ticket_handle);
            }
        }

    private:
        std::weak_ptr<OperationalQueue> _queue;
        std::weak_ptr<OperationalQueue::_TicketHandle> _ticket_handle;
        QueueExecutor::OperationType _operation_type;
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

        clear(OperationType::CANCELABLE);
    }

    void QueueExecutor::wait_all() {
        clear(OperationType::NON_CANCELABLE);
    }

    void QueueExecutor::cancel_all() {
        clear(OperationType::CANCELABLE);
    }

    huesdk::Operation QueueExecutor::execute(std::function<void()> invocable, OperationType operation_type /* = OperationType::CANCELABLE */) {
        {
            std::lock_guard<std::mutex> lock(_impl->sync);
            if (_impl->closing) {
                return Operation{std::make_shared<QueueExecutorOperation>()};
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

            _impl->tickets.push_back(Impl::TicketInfo{ticket, operation_type == OperationType::NON_CANCELABLE});
            _impl->queue->schedule_ticket(ticket);
            posted = true;
        } while (false);

        if (!posted) {
            _impl->queue->discard_ticket(ticket);
        }

        return Operation{std::make_shared<QueueExecutorOperation>(_impl->queue, ticket, operation_type)};
    }

    void QueueExecutor::clear(QueueExecutor::OperationType policy) {
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
                if (!it->wait_on_cleanup && policy == OperationType::CANCELABLE) {
                    queue->discard_ticket(it->ticket);
                    closed_tickets.push_back(it->ticket);
                }
            }

            // forward order for execution
            for (auto it = std::begin(tickets); it != std::end(tickets); ++it) {
                if (it->wait_on_cleanup || policy == OperationType::NON_CANCELABLE) {
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
}  // namespace support
