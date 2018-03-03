/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#include <utility>

#include "support/threading/QueueDispatcher.h"
#include "support/threading/DetachingOperationalQueue.h"
#include "support/threading/QueueExecutor.h"

namespace support {

    QueueDispatcher::QueueDispatcher(std::shared_ptr<OperationalQueue> queue, bool wait_all)
            : _executor(std::unique_ptr<QueueExecutor>(new QueueExecutor(queue)))
            , _wait_all(wait_all) {
    }

    QueueDispatcher::~QueueDispatcher() {
    }

    huesdk::Operation QueueDispatcher::post(std::function<void()> invocation) {
        return _executor->execute(std::move(invocation), _wait_all ?
            QueueExecutor::OperationType::NON_CANCELABLE : QueueExecutor::OperationType::CANCELABLE);
    }

    std::shared_ptr<QueueDispatcher> QueueDispatcher::global() {
        auto queue = global_dispatch_queue();
        static auto s_instance = std::make_shared<QueueDispatcher>(queue);
        return s_instance;
    }

    void QueueDispatcher::shutdown() {
        _executor->shutdown();
    }

    std::shared_ptr<OperationalQueue> global_dispatch_queue() {
        static auto s_instance = std::make_shared<OperationalQueue>("glob-dispatch");
        return s_instance;
    }

}  // namespace support
