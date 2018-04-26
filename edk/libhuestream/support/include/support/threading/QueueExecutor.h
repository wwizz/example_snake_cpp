/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/
#pragma once

#include <functional>
#include <memory>

#include "hueutilities/Operation.h"

namespace support {

    class OperationalQueue;

    /**
     * Provides functionality for executing functions withing an operational queue.
     */
    class QueueExecutor final {
    public:
        enum class OperationType {
            CANCELABLE,
            NON_CANCELABLE
        };

        /**
         * Constructor.
         * @param queue Operational queue used to execute given requests
         */
        explicit QueueExecutor(std::shared_ptr<OperationalQueue> queue);
        /**
         * Destructor.
         * Cancels all posted calls and waits for non cancelable executions to complete.
         */
        ~QueueExecutor();

        QueueExecutor(const QueueExecutor&) = delete;
        QueueExecutor& operator=(const QueueExecutor&) = delete;

        /**
         * Gets global executor working on the global OperationalQueue
         * @return global executor instance pointer
         */
        static std::shared_ptr<QueueExecutor> global();

        /**
         * Requests item execution on the operational queue. Item is put the the end of the queue.
         * @param invocable Function to be executed
         * @param operation_type Specifies if executor allowed to disacard this request on cancelation and destruction.
         */
        huesdk::Operation execute(std::function<void()> invocable, OperationType operation_type = OperationType::CANCELABLE);

        /**
         * Wait for all requests to be executed.
         * Blocks until there is no requests to process.
         * All incoming requests during wait_all() will be also processed.
         */
        void wait_all();

/**
         * Discards cancelable requests and waits other requests to be executed.
         * Blocks until there is no requests to process.
         * All incoming requests during cancel_all() will be also processed.
         */
        void cancel_all();

        /**
         * Stops all incoming requests processing, discards cancelable requests and waits other requests to be executed.
         * Blocks until there is no requests to process.
         */
        void shutdown();

        /**
         * Gets operational queue used for requests execution.
         * @return working operational queue pointer
         */
        std::shared_ptr<OperationalQueue> queue() const;

    private:
        void clear(OperationType policy);

        struct Impl;
        std::shared_ptr<Impl> _impl;
    };

    std::shared_ptr<OperationalQueue> global_execution_queue();
}  // namespace support

