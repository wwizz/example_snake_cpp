/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/
#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "support/threading/Thread.h"

namespace support {

    class OperationalQueue {
    public:
        OperationalQueue();
        explicit OperationalQueue(std::string thread_name);
        virtual ~OperationalQueue();

        OperationalQueue(const OperationalQueue&) = delete;
        OperationalQueue& operator=(const OperationalQueue&) = delete;

        struct _TicketHandle {};
        using TicketHandle = std::shared_ptr<_TicketHandle>;

        TicketHandle create_ticket(std::function<void()> invocable);

        void schedule_ticket(const TicketHandle& ticket);
        void discard_ticket(const TicketHandle& ticket);
        void wait_ticket(const TicketHandle& ticket);

        bool has_ticket(const TicketHandle& ticket);
        size_t ticket_count();

        bool run_recursive_process_tickets_loop(const std::function<bool()>& break_condition);

    protected:
        explicit OperationalQueue(std::string thread_name, bool auto_start);

        struct TicketDescriptor {
            TicketHandle handle;
            std::function<void()> invocable;
            std::promise<void> completed_promise;
            std::shared_future<void> completed_future;
            bool was_scheduled = false;
        };

        using TicketDescriptorPtr = std::shared_ptr<TicketDescriptor>;

        void start_processing(std::function<void(TicketDescriptorPtr)> ticket_executor = {});
        void stop_processing();

        Thread eject_processing_thread();

    private:
        struct State {
            Thread processing_thread;
            std::mutex sync_mutex;
            std::condition_variable sync_condition;

            std::list<TicketDescriptorPtr> processing_queue;
            std::map<TicketHandle, TicketDescriptorPtr> registry;
            std::list<TicketDescriptorPtr> current_ticket_stack;
            bool stop_processing_requested = false;

            std::function<void(TicketDescriptorPtr)> ticket_executor;
            std::string thread_name;
        };

        std::shared_ptr<State> _state;

        static void processing_loop(std::shared_ptr<State> state, std::function<bool()> recursion_break_condition);
    };

}  // namespace support

