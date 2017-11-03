/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#ifndef THREADING_CONCURRENTEXECUTOR_H_
#define THREADING_CONCURRENTEXECUTOR_H_

#include "threading/ThreadPool.h"

namespace huesdk_lib {

    class ConcurrentExecutor : public ThreadPool {
    public:
        static const size_t WORKERS = 4;

        static ConcurrentExecutor& global() {
            static ConcurrentExecutor _instance;
            return _instance;
        }

        explicit ConcurrentExecutor(bool finish_tasks_on_close = false) : ThreadPool{WORKERS, finish_tasks_on_close} {};
    };

}  // namespace huesdk_lib

#endif  // THREADING_CONCURRENTEXECUTOR_H_
