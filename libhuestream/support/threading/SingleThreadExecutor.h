/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#ifndef THREADING_SINGLETHREADEXECUTOR_H_
#define THREADING_SINGLETHREADEXECUTOR_H_

#include "threading/ThreadPool.h"

namespace huesdk_lib {

    class SingleThreadExecutor : public ThreadPool {
    public:
        explicit SingleThreadExecutor(bool finish_tasks_on_close = false) : ThreadPool{1, finish_tasks_on_close} {};
    };

}  // namespace huesdk_lib

#endif  // THREADING_SINGLETHREADEXECUTOR_H_

