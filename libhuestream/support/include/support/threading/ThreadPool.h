/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <vector>
#include <mutex>
#include <string>
#include <queue>
#include <type_traits>
#include <utility>

#include "support/threading/Thread.h"
#include "support/std/types/VectorOperations.h"

#define FUNCTION_RETURN_TYPE(Function, Args) typename std::result_of<Function(Args...)>::type

namespace support {

    using ThreadPoolTask = std::function<void ()>;

    class ThreadPool {
    public:
        /**
         @param finish_tasks_on_close on close, all tasks are executed before exiting
         @param name thread pool name applied to the workers
         */
        explicit ThreadPool(size_t workers, bool finish_tasks_on_close = false, std::string name = {});

        /**
         Will block until the thread pool is closed
         */
        ~ThreadPool();

        template<typename Function, typename... Args>
        std::future<FUNCTION_RETURN_TYPE(Function, Args)> add_task(Function&& function, Args&&... args) {
            std::future<FUNCTION_RETURN_TYPE(Function, Args)> future;

            typedef std::packaged_task<FUNCTION_RETURN_TYPE(Function, Args)()> TaskAsync;

            std::unique_lock<std::mutex> tasks_lock(_tasks_mutex);
            
            if (!_close) {
                auto task = std::bind(std::forward<Function>(function), std::forward<Args>(args)...);

                std::shared_ptr<TaskAsync> task_async = std::make_shared<TaskAsync>(task);

                future = task_async->get_future();

                _tasks.push([task_async] () -> void {
                    (*task_async)();
                });

                _tasks_condition.notify_one();
            }
            
            return future;
        }
        
        /**
         Close the thread pool, which will block until the thread pool is closed
         @param  cancel_tasks_delegate      The delegate that should be used for canceling of the tasks.
                                            all tasks are cancelled by caller thread.
         */
        void close(std::function<void()> cancel_tasks_delegate = std::function<void()>());

        size_t number_of_threads() const;
        
    private:
        void event_loop();

        std::vector<Thread>        _threads;

        std::queue<ThreadPoolTask> _tasks;
        std::mutex                 _tasks_mutex;
        std::condition_variable    _tasks_condition;
        std::atomic<bool>          _close;
        bool                       _should_finish_tasks_on_close;
        
        /**
         Check whether tasks should be executed. 
         Conditions:
         - close() or destructor has not been called
         OR:
         - threadpool is configured that all tasks should be executed on closing
         - tasks list is not empty
         */
        bool should_execute_tasks() const;
    };

}  // namespace support

