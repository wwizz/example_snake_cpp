/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <mutex>
#include <vector>
#include <memory>

#include "support/logging/Log.h"

#include "support/std/types/VectorOperations.h"
#include "support/threading/Job.h"
#include "support/threading/QueueDispatcher.h"
#include "support/threading/DetachingOperationalQueue.h"

#include "bridgediscovery/BridgeDiscoveryResult.h"

#include "method/IBridgeDiscoveryMethod.h"

namespace huesdk {
    template<typename TaskT>
    class BridgeDiscoveryMethodBase : public IBridgeDiscoveryMethod {
    public:
        using TaskType = TaskT;
        using MethodResultCallback = std::function<void(const std::vector<std::shared_ptr<BridgeDiscoveryResult>> &)>;

        BridgeDiscoveryMethodBase() : _callback(nullptr), _dispatcher(support::global_dispatch_queue()) {
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
         @return True when job started successfully, false otherwise
         */
        virtual bool method_search(const MethodResultCallback &callback) = 0;

        std::unique_ptr<support::Job<TaskType>> _job;

    private:
        std::mutex _mutex;
        std::shared_ptr<IBridgeDiscoveryCallback> _callback;
        support::QueueDispatcher _dispatcher;
    };

    template<typename TaskType>
    bool BridgeDiscoveryMethodBase<TaskType>::is_searching() {
        std::lock_guard<std::mutex> lock_guard{_mutex};
        return _job != nullptr && _job->state() == support::JobState::Running;
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

        auto search_started = method_search([this, callback](const std::vector<std::shared_ptr<BridgeDiscoveryResult>> &results) {
            _dispatcher.post([callback, results]() {
                (*callback)(results, BRIDGE_DISCOVERY_RETURN_CODE_SUCCESS);
            });
        });

        if (!search_started) {
            _dispatcher.post([callback]() {
                (*callback)({}, BRIDGE_DISCOVERY_RETURN_CODE_BUSY);
            });
        }
    }

    template<typename TaskType>
    void BridgeDiscoveryMethodBase<TaskType>::stop() {
        std::lock_guard<std::mutex> lock_guard{_mutex};
        if (_job != nullptr && _job->cancel()) {
            _dispatcher.post([this] {
                (*_callback)(_job->get()->get_result(), BRIDGE_DISCOVERY_RETURN_CODE_STOPPED);
            });
        }
    }
}  // namespace huesdk

