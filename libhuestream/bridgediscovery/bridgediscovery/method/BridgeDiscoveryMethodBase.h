/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_BRIDGEDISCOVERYMETHODBASE_H_
#define BRIDGEDISCOVERY_METHOD_BRIDGEDISCOVERYMETHODBASE_H_

#include <mutex>
#include <vector>
#include <memory>

#include "logging/Log.h"

#include "std/types/Vector.h"
#include "threading/Job.h"
#include "threading/QueueDispatcher.h"
#include "threading/DetachingOperationalQueue.h"
#include "bridgediscovery/method/IBridgeDiscoveryMethod.h"
#include "bridgediscovery/BridgeDiscoveryResult.h"

namespace huesdk {
    template<typename TaskT>
    class BridgeDiscoveryMethodBase : public IBridgeDiscoveryMethod {
    public:
        using TaskType = TaskT;
        using MethodResultCallback = std::function<void(const std::vector<BridgeDiscoveryResult> &)>;

        BridgeDiscoveryMethodBase() : _callback(nullptr), _dispatcher(huesdk_lib::global_dispatch_queue()) {
        }

        /**
         @see IBridgeDiscoveryMethod.h
        */
        void search(std::shared_ptr<IBridgeDiscoveryCallback> callback) final;

        /**
         @see IBridgeDiscoveryMethod.h
         */
        void stop() final;

        /**
         @see IBridgeDiscoveryMethod.h
         */
        bool is_searching() final;

        ~BridgeDiscoveryMethodBase() override {
            stop();
        }

    protected:
        /**
         Starts method search job
         @param callback The callback which will be called when method finishes searching or stops
                Contains the search results. Will be called only once.
         @return Job state enum. If job has started successfully, its value should be JobState::Running
         */
        virtual huesdk_lib::JobState method_search(const MethodResultCallback &callback) = 0;

        std::unique_ptr<huesdk_lib::Job<TaskType>> _job;

    private:
        std::mutex _mutex;
        std::shared_ptr<IBridgeDiscoveryCallback> _callback;
        huesdk_lib::QueueDispatcher _dispatcher;
    };

    template<typename TaskType>
    bool BridgeDiscoveryMethodBase<TaskType>::is_searching() {
        std::lock_guard<std::mutex> lock_guard{_mutex};
        return _job != nullptr && _job->state() == huesdk_lib::JobState::Running;
    }

    template<typename TaskType>
    void BridgeDiscoveryMethodBase<TaskType>::search(std::shared_ptr<IBridgeDiscoveryCallback> callback) {
        std::lock_guard<std::mutex> lock_guard{_mutex};

        if (callback == nullptr) {
            HUE_LOG << HUE_CORE << HUE_WARN << "BridgeDiscoveryMethodBase: no callback defined " << HUE_ENDL;
            return;
        }

        _callback = callback;

        if (_job != nullptr) {
            _dispatcher.post([callback]() {
                (*callback)({}, BRIDGE_DISCOVERY_RETURN_CODE_BUSY);
            });
            return;
        }

        auto run_result = method_search([this, callback](const std::vector<BridgeDiscoveryResult> &results) {
            _dispatcher.post([callback, results]() {
                (*callback)(results, BRIDGE_DISCOVERY_RETURN_CODE_SUCCESS);
            });
        });

        if (run_result != huesdk_lib::JobState::Running) {
            _dispatcher.post([callback]() {
                (*callback)({}, BRIDGE_DISCOVERY_RETURN_CODE_BUSY);
            });
        }
    }

    template<typename TaskType>
    void BridgeDiscoveryMethodBase<TaskType>::stop() {
        std::lock_guard<std::mutex> lock_guard{_mutex};
        if (_job != nullptr && _job->state() == huesdk_lib::JobState::Running) {
            if (_job->cancel() == huesdk_lib::JobState::Canceled) {
                _dispatcher.post([this] {
                    (*_callback)(_job->get()->get_result(), BRIDGE_DISCOVERY_RETURN_CODE_STOPPED);
                });
            }
        }
    }
}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_METHOD_BRIDGEDISCOVERYMETHODBASE_H_
