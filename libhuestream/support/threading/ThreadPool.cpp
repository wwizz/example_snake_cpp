/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <algorithm>
#include <string>

#include "threading/ThreadPool.h"

namespace huesdk_lib {

    ThreadPool::ThreadPool(size_t workers, bool finish_tasks_on_close, std::string name)
            : _close(false)
            , _should_finish_tasks_on_close(finish_tasks_on_close) {
        // Initialize the workers
        for (size_t i = 0; i < workers; i++) {
            const auto thread_name = name.empty() ? name : name + "-" + std::to_string(i + 1);
            _threads.emplace_back(Thread(thread_name, std::bind(&ThreadPool::event_loop, this)));
        }
    }

    void ThreadPool::event_loop() {
        std::unique_lock<std::mutex> tasks_lock(_tasks_mutex);

        // Keep handling tasks until the thread pool is closed
        while (should_execute_tasks()) {
            _tasks_condition.wait(tasks_lock, [&] () -> bool {
                // Wait until there is a new task or the thread pool closed
                return (!_tasks.empty() || _close);
            });

            if (should_execute_tasks()) {
                // Get task from the queue
                ThreadPoolTask task = _tasks.front();
                // Remove task from the queue
                _tasks.pop();

                tasks_lock.unlock();

                // Execute the task
                task();

                tasks_lock.lock();
            }
        }
    }
    
    ThreadPool::~ThreadPool() {
        // Close the thread pool if not yet done
        close();
    }

    void ThreadPool::close(std::function<void()> cancel_tasks_delegate /* = std::function<void()>() */) {
        if (_close) {
            return;
        }

        {
            std::lock_guard<std::mutex> tasks_lock(_tasks_mutex);

            // Close the thread pool
            _close = true;

            _tasks_condition.notify_all();
        }

        if (cancel_tasks_delegate) {
            cancel_tasks_delegate();
        }

        for (Thread& worker : _threads) {
            // Wait until the worker thread is finished
            worker.join();
        }
    }
    
    bool ThreadPool::should_execute_tasks() const {
        return !_close || (!_tasks.empty() && _should_finish_tasks_on_close);
    }

    size_t ThreadPool::number_of_threads() const {
        return _threads.size();
    }

}  // namespace huesdk_lib
