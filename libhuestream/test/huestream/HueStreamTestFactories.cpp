/*******************************************************************************
 Copyright (c) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <test/huestream/HueStreamTestFactories.h>

namespace huestream {

template<>
MessageDispatcherPtr huestream_test_factory<MessageDispatcherPtr>() {
    return HueStreamTestFactories<MessageDispatcherPtr>::DoCreate();
}

template<>
StreamPtr huestream_test_factory<StreamPtr,
                                 StreamSettingsPtr,
                                 AppSettingsPtr,
                                 TimeManagerPtr,
                                 ConnectorPtr>(StreamSettingsPtr settings,
                                               AppSettingsPtr appSettings,
                                               TimeManagerPtr timeManager,
                                               ConnectorPtr connector) {
    return HueStreamTestFactories<StreamPtr,
                                  StreamSettingsPtr,
                                  AppSettingsPtr,
                                  TimeManagerPtr,
                                  ConnectorPtr>::DoCreate();

};

template<>
HttpClientPtr huestream_test_factory<huestream::HttpClientPtr>(){
    return HueStreamTestFactories<HttpClientPtr>::DoCreate();
}

template<>
TimeManagerPtr huestream_test_factory<TimeManagerPtr>() {
    return HueStreamTestFactories<TimeManagerPtr>::DoCreate();
}

template<>
ConnectionMonitorPtr huestream_test_factory<ConnectionMonitorPtr,
                                            HttpClientPtr, AppSettingsPtr>(
    huestream::HttpClientPtr httpClient, AppSettingsPtr appSettings){
    return HueStreamTestFactories<ConnectionMonitorPtr, HttpClientPtr, AppSettingsPtr>::DoCreate();
};

template<>
MixerPtr huestream_test_factory<MixerPtr>() {
    return HueStreamTestFactories<MixerPtr>::DoCreate();
}

template<>
ConnectorPtr huestream_test_factory<ConnectorPtr, ConfigPtr>(ConfigPtr config) {
    return HueStreamTestFactories<ConnectorPtr, ConfigPtr>::DoCreate();
};

template<>
BridgeStorageAccessorPtr huestream_test_factory<BridgeStorageAccessorPtr,
                                                   const std::string &,
                                                   BridgeSettingsPtr>(const std::string &fileName,
                                                                      BridgeSettingsPtr bridgeSettings) {
    return HueStreamTestFactories<BridgeStorageAccessorPtr,
                                  const std::string &,
                                  BridgeSettingsPtr>::DoCreate();
}

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
                                                            BridgeStorageAccessorPtr bridgeStorageAccessor) {
    return HueStreamTestFactories<ConnectPtr,
                                  HttpClientPtr,
                                  MessageDispatcherPtr,
                                  BridgeSettingsPtr,
                                  AppSettingsPtr,
                                  StreamPtr,
                                  BridgeStorageAccessorPtr>::DoCreate();
}

template<>
MessageTranslatorPtr huestream_test_factory<MessageTranslatorPtr, std::string>(std::string language) {
    return HueStreamTestFactories<MessageTranslatorPtr,
                                  std::string>::DoCreate();

}

template<>
BasicGroupLightControllerPtr huestream_test_factory<BasicGroupLightControllerPtr,
    HttpClientPtr>(
        huestream::HttpClientPtr httpClient) {
    return HueStreamTestFactories<BasicGroupLightControllerPtr, HttpClientPtr>::DoCreate();
};


}