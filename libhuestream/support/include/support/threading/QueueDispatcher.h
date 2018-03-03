/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <future>
#include <queue>

#include "hueutilities/Operation.h"

namespace support {
    class OperationalQueue;
    class QueueExecutor;

    /**
     * Provides functionality for dispatching information to external listeners through operational queue.
     */
    class QueueDispatcher final {
    public:
        /**
         * Constructor.
         * @param queue Dispatch queue used to execute given requests
         * @param wait_all specifies if the dispatcher should wait posted invocations on shutdown.
         */
        explicit QueueDispatcher(std::shared_ptr<OperationalQueue> queue, bool wait_all = true);

        /**
         * Waits for posted invocations to be discarded or finished.
         */
        ~QueueDispatcher();

        QueueDispatcher(const QueueDispatcher&) = delete;
        QueueDispatcher& operator=(const QueueDispatcher&) = delete;

        /**
         * Gets global dispatcher
         * @return global dispatcher instance pointer
         */
        static std::shared_ptr<QueueDispatcher> global();

        /**
         * Requests invocation to be performed on the dispatcher thread.
         * @param invocation Function to be executed
         */
        huesdk::Operation post(std::function<void()> invocation);

        /**
         * Stops all incoming requests processing, waits for posted invocations to be discarded or finished.
         * Blocks until there is no requests to process.
         */
        void shutdown();

    private:
        std::unique_ptr<QueueExecutor> _executor;
        bool _wait_all = true;
    };

    std::shared_ptr<OperationalQueue> global_dispatch_queue();

}  // namespace support

