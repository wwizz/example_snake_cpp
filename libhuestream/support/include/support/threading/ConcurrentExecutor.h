/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#pragma once

#include "support/threading/ThreadPool.h"

namespace support {

    class ConcurrentExecutor : public ThreadPool {
    public:
        static const size_t WORKERS = 4;

        static ConcurrentExecutor& global() {
            static ConcurrentExecutor _instance;
            return _instance;
        }

        explicit ConcurrentExecutor(bool finish_tasks_on_close = false) : ThreadPool{WORKERS, finish_tasks_on_close} {};
    };

}  // namespace support

