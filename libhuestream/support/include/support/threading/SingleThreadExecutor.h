/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#pragma once

#include "support/threading/ThreadPool.h"

namespace support {

    class SingleThreadExecutor : public ThreadPool {
    public:
        explicit SingleThreadExecutor(bool finish_tasks_on_close = false) : ThreadPool{1, finish_tasks_on_close} {};
    };

}  // namespace support

