/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_MESSAGEDISPATCHER_H_
#define HUESTREAM_CONNECT_MESSAGEDISPATCHER_H_

#include <atomic>
#include <condition_variable>
#include <vector>
#include <memory>
#include <mutex>

#include "threading/Thread.h"
#include "IMessageDispatcher.h"

namespace huestream {

    class MessageDispatcher : public IMessageDispatcher {
    public:
        MessageDispatcher();

        void Queue(DispatchAction action) override;

        void Execute(bool useThisTread = false) override;

        void Loop();

        void Stop() override;

    protected:
        std::vector<DispatchAction> _queue;
        std::mutex _mutex;
        std::condition_variable _condition;
        bool _isRunning;
        std::unique_ptr<huesdk_lib::Thread> _thread;
        void WaitUntilStarted();
    };

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_MESSAGEDISPATCHER_H_
