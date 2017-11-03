/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "bridgediscovery/BridgeDiscovery.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodProvider.h"
#include "method/BridgeDiscoveryMethodUtil.h"
#include "tasks/BridgeDiscoveryCheckIpArrayTask.h"
#include "logging/Log.h"
#include "threading/QueueDispatcher.h"

#if !defined(SDK_DEBUG) && !defined(SDK_TEST)
#include "bridgediscovery/method/BridgeDiscoveryMethodProvider.h"  // NOLINT

#define BRIDGE_DISCOVERY_METHOD_PROVIDER huesdk::BridgeDiscoveryMethodProvider
// Debug interface
#elif defined(SDK_DEBUG) && !defined(SDK_TEST)

#define BRIDGE_DISCOVERY_METHOD_PROVIDER huesdk::BridgeDiscoveryMethodProvider
// Test interface, which includes debug additions
#else
#include "bridgediscovery/method/_test/BridgeDiscoveryMethodProviderDelegator.h"  // NOLINT

#define BRIDGE_DISCOVERY_METHOD_PROVIDER huesdk::BridgeDiscoveryMethodProviderDelegator
#endif

using std::vector;
using std::unique_ptr;
using std::string;
using huesdk_lib::QueueDispatcher;

class BridgeDiscoveryResult;

namespace huesdk {
    BridgeDiscovery::~BridgeDiscovery() {
        HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscovery: destructor" << HUE_ENDL;

        stop();
    }

    void BridgeDiscovery::search(IBridgeDiscoveryCallback *callback) {
        search(DISCOVERY_OPTION_UPNP | DISCOVERY_OPTION_NUPNP, callback);
    }

    void BridgeDiscovery::search(int options, IBridgeDiscoveryCallback *callback) {
        if (callback == nullptr) {
            HUE_LOG << HUE_CORE << HUE_WARN << "BridgeDiscovery: no callback defined " << HUE_ENDL;
            return;
        }

        if (is_searching()) {
            HUE_LOG << HUE_CORE << HUE_DEBUG
                    << "BridgeDiscovery: trying to start a search while a search is already in progress" << HUE_ENDL;

            QueueDispatcher::global()->post([callback] {
                (*callback)({}, BRIDGE_DISCOVERY_RETURN_CODE_BUSY);
            });
            return;
        }

        auto discovery_methods = get_discovery_methods(options);

        if (discovery_methods.empty()) {
            HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscovery: no discovery methods provided" << HUE_ENDL;
            QueueDispatcher::global()->post([callback] {
                (*callback)({}, BRIDGE_DISCOVERY_RETURN_CODE_MISSING_DISCOVERY_METHODS);
            });
            return;
        }

        HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscovery: start searching" << HUE_ENDL;
        _discovery_job = huesdk_lib::create_job<BridgeDiscoveryTask>(std::move(discovery_methods));
        _discovery_job->run([callback](BridgeDiscoveryTask *task) {
            auto results = task->get_results();
            QueueDispatcher::global()->post([callback, results]() {
                (*callback)(results, BRIDGE_DISCOVERY_RETURN_CODE_SUCCESS);
            });
        });

        _callback = callback;
    }

    bool BridgeDiscovery::is_searching() {
        if (_discovery_job != nullptr) {
            return _discovery_job->state() == huesdk_lib::JobState::Running;
        }
        return false;
    }

    void BridgeDiscovery::stop() {
        HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscovery: stopping search" << HUE_ENDL;

        if (_discovery_job != nullptr && _discovery_job->state() == huesdk_lib::JobState::Running) {
            if (_discovery_job->cancel() == huesdk_lib::JobState::Canceled) {
                auto callback = _callback;
                auto result = _discovery_job->get()->get_results();
                QueueDispatcher::global()->post([callback, result] {
                    (*callback)(result, BRIDGE_DISCOVERY_RETURN_CODE_STOPPED);
                });
            }
        }
    }

    vector<unique_ptr<IBridgeDiscoveryMethod>> BridgeDiscovery::get_discovery_methods(int options) {
        vector<unique_ptr<IBridgeDiscoveryMethod>> discovery_methods;

        // NOTE: IPSCAN must go first, because its search will contain all needed bridge info. Possible following (N)UPNP search results of the same bridge will be discarded.
        auto discovery_options = {
                DISCOVERY_OPTION_IPSCAN,
                DISCOVERY_OPTION_UPNP,
                DISCOVERY_OPTION_NUPNP
        };

        for (const auto &discovery_option : discovery_options) {
            if (options & discovery_option) {
                auto method = BRIDGE_DISCOVERY_METHOD_PROVIDER::get_discovery_method(discovery_option);
                if (method != nullptr) {
                    discovery_methods.push_back(std::move(method));
                } else {
                    HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscovery: nullptr provided as search method"
                            << discovery_option << HUE_ENDL;
                }
            }
        }

        return discovery_methods;
    }
}  // namespace huesdk
