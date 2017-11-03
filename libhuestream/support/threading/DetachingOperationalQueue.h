/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#ifndef THREADING_DETACHINGOPERATIONALQUEUE_H_
#define THREADING_DETACHINGOPERATIONALQUEUE_H_

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <queue>
#include <utility>

#include "threading/Thread.h"
#include "threading/OperationalQueue.h"

namespace huesdk_lib {

    class DetachingOperationalQueue : public OperationalQueue {
    public:
        DetachingOperationalQueue();
        ~DetachingOperationalQueue() override;

        explicit DetachingOperationalQueue(std::string thread_name);
        explicit DetachingOperationalQueue(std::string thread_name, std::function<void(Thread&&)> detach_delegate, std::chrono::milliseconds max_dispatch_time);

    private:
        struct State;

        static void execute_ticket(std::shared_ptr<State> state, TicketDescriptorPtr ticket);
        void processing_watchdog_loop();

        Thread _watchdog_thread;

        struct State {
            std::mutex sync_mutex;
            std::condition_variable sync_condition;

            std::queue<std::pair<TicketHandle, std::shared_future<void>>> running_tickets;
            bool stop_watchdog_requested = false;

            std::function<void(Thread&&)> detach_delegate;
            std::chrono::milliseconds max_dispatch_time;
        };

        std::shared_ptr<State> _state;
    };



}  // namespace huesdk_lib

#endif  // THREADING_DETACHINGOPERATIONALQUEUE_H_
