/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef LIBHUESTREAM_TEST_FACTORY_OVERRIDE_H_
#define LIBHUESTREAM_TEST_FACTORY_OVERRIDE_H_

#include <util/Factory.h>
#include <huestream/connect/IMessageDispatcher.h>
#include <huestream/connect/IConnectionMonitor.h>
#include <vector>
#include <gtest/gtest.h>
#include <huestream/stream/IStream.h>
#include <huestream/stream/StreamSettings.h>
#include <huestream/config/AppSettings.h>
#include <huestream/common/time/ITimeManager.h>
#include <huestream/effect/IMixer.h>
#include <huestream/config/Config.h>
#include <huestream/connect/IBridgeStorageAccessor.h>
#include <huestream/connect/Connect.h>
#include <huestream/connect/IBasicGroupLightController.h>

namespace huestream {

template<typename Product, typename... Args>
Product huestream_test_factory(Args... args);

template<>
MessageDispatcherPtr huestream_test_factory<MessageDispatcherPtr>();

template<>
StreamPtr huestream_test_factory<StreamPtr,
                                 StreamSettingsPtr,
                                 AppSettingsPtr,
                                 TimeManagerPtr,
                                 ConnectorPtr>(StreamSettingsPtr settings,
                                               AppSettingsPtr appSettings,
                                               TimeManagerPtr timeManager,
                                               ConnectorPtr connector);

template<>
huestream::HttpClientPtr huestream_test_factory<huestream::HttpClientPtr>();

template<>
TimeManagerPtr huestream_test_factory<TimeManagerPtr>();

template<>
ConnectionMonitorPtr huestream_test_factory<ConnectionMonitorPtr,
                                            HttpClientPtr, AppSettingsPtr>(
    huestream::HttpClientPtr httpClient, AppSettingsPtr appSettings);


template<>
MixerPtr huestream_test_factory<MixerPtr>();

template<>
ConnectorPtr huestream_test_factory<ConnectorPtr, ConfigPtr>(ConfigPtr config);

template<>
BridgeStorageAccessorPtr huestream_test_factory<BridgeStorageAccessorPtr,
                                                const std::string &,
                                                BridgeSettingsPtr>(const std::string &fileName,
                                                                   BridgeSettingsPtr bridgeSettings);

template<>
ConnectPtr huestream_test_factory<ConnectPtr,
                                  HttpClientPtr,
                                  MessageDispatcherPtr,
                                  BridgeSettingsPtr,
                                  AppSettingsPtr,
                                  StreamPtr,
                                  BridgeStorageAccessorPtr>(HttpClientPtr httpClient,
                                                            MessageDispatcherPtr messageDispatcher,
                                                            BridgeSettingsPtr bridgeSettings,
                                                            AppSettingsPtr appSettings,
                                                            StreamPtr stream,
                                                            BridgeStorageAccessorPtr bridgeStorageAccessor);

template<>
MessageTranslatorPtr huestream_test_factory<MessageTranslatorPtr, std::string>(std::string language);

template<>
BasicGroupLightControllerPtr huestream_test_factory<BasicGroupLightControllerPtr,
    HttpClientPtr>(huestream::HttpClientPtr httpClient);


template<typename Product, typename... Args>
class HueStreamTestFactories {
 public:
    typedef std::function<Product()> CreateCallback;


    static void Override(CreateCallback cb) {
        std::function<Product(Args... args)> x = huestream_test_factory<Product, Args...>;
        huesdk_lib::Factory<Product, Args...>::SetFactoryMethod(x);
        callback() = cb;
    }

    static void Clear() {
        huesdk_lib::Factory<Product>::SetFactoryMethod(nullptr);
    }

    static Product DoCreate() {
        return callback()();
    }

    static CreateCallback &callback() {
        static CreateCallback _callback;
        return _callback;
    }
};


}// namespace huestream
#endif     // LIBHUESTREAM_TEST_FACTORY_OVERRIDE_H_
