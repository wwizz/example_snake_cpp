
/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESDK_HUESTREAMDEFAULTFACTORIES_H
#define HUESDK_HUESTREAMDEFAULTFACTORIES_H

#include <util/Factory.h>
#include <huestream/connect/IBridgeStorageAccessor.h>
#include <huestream/connect/BridgeFileStorageAccessor.h>
#include <huestream/connect/BridgeStreamingChecker.h>
#include <huestream/common/language/DummyTranslator.h>
#include <huestream/common/http/HttpClient.h>
#include <huestream/common/time/TimeManager.h>
#include <huestream/effect/Mixer.h>
#include <huestream/stream/DtlsEntropyProvider.h>
#include <huestream/HueStreamFactories.h>
#include <huestream/stream/DtlsConnector.h>
#include <huestream/stream/UdpConnector.h>

#include <string>
#include <memory>

template<>
EntropyProviderBasePtr huesdk_lib_default_factory<EntropyProviderBasePtr>() {
    return std::make_shared<huestream::DtlsEntropyProvider>();
}

template<>
huestream::BridgeStorageAccessorPtr huesdk_lib_default_factory<huestream::BridgeStorageAccessorPtr,
                                                    const std::string &,
                                                    huestream::BridgeSettingsPtr>(const std::string &fileName,
                                                                               huestream::BridgeSettingsPtr bridgeSettings) {
    return std::make_shared<huestream::BridgeFileStorageAccessor>(fileName, bridgeSettings);
}

template<>
huestream::MixerPtr huesdk_lib_default_factory<huestream::MixerPtr>() {
    return std::make_shared<huestream::Mixer>();
}

template<>
huestream::MessageTranslatorPtr huesdk_lib_default_factory<huestream::MessageTranslatorPtr, std::string>(std::string language) {
    return std::make_shared<huestream::DummyTranslator>(language);
}

template<>
huestream::ConnectPtr huesdk_lib_default_factory<huestream::ConnectPtr,
                                                    huestream::HttpClientPtr,
                                                    huestream::MessageDispatcherPtr,
                                                    huestream::BridgeSettingsPtr,
                                                    huestream::AppSettingsPtr,
                                                    huestream::StreamPtr,
                                                    huestream::BridgeStorageAccessorPtr>(
    huestream::HttpClientPtr httpClient,
    huestream::MessageDispatcherPtr messageDispatcher,
    huestream::BridgeSettingsPtr bridgeSettings,
    huestream::AppSettingsPtr appSettings,
    huestream::StreamPtr stream,
    huestream::BridgeStorageAccessorPtr bridgeStorageAccessor) {
    return std::make_shared<huestream::Connect>(
        httpClient,
        messageDispatcher,
        bridgeSettings,
        appSettings,
        stream,
        bridgeStorageAccessor);
}

template<>
huestream::StreamPtr huesdk_lib_default_factory<huestream::StreamPtr,
                                                huestream::StreamSettingsPtr,
                                                huestream::AppSettingsPtr,
                                                huestream::TimeManagerPtr,
                                                huestream::ConnectorPtr>(
    huestream::StreamSettingsPtr streamSettings,
    huestream::AppSettingsPtr appSettings,
    huestream::TimeManagerPtr timeManager,
    huestream::ConnectorPtr connector) {
    return std::make_shared<huestream::Stream>(
        streamSettings,
        appSettings,
        timeManager,
        connector);
}

template<>
huestream::HttpClientPtr huesdk_lib_default_factory<huestream::HttpClientPtr>() {
    return std::make_shared<huestream::HttpClient>();
}


template<>
huestream::ConnectionMonitorPtr huesdk_lib_default_factory<huestream::ConnectionMonitorPtr,
                                                           huestream::HttpClientPtr, huestream::AppSettingsPtr>(
    huestream::HttpClientPtr httpClient, huestream::AppSettingsPtr appSettings) {
    return std::make_shared<huestream::ConnectionMonitor>(
        std::make_shared<huestream::BridgeStreamingChecker>(
            std::make_shared<huestream::FullConfigRetriever>(httpClient, appSettings->UseForcedActivation())));
}

template<>
huestream::TimeManagerPtr huesdk_lib_default_factory<huestream::TimeManagerPtr>() {
    return std::make_shared<huestream::TimeManager>();
}

template<>
huestream::MessageDispatcherPtr huesdk_lib_default_factory<huestream::MessageDispatcherPtr>() {
    return std::make_shared<huestream::MessageDispatcher>();
}

template<>
huestream::ConnectorPtr huesdk_lib_default_factory<huestream::ConnectorPtr, huestream::ConfigPtr>(
    huestream::ConfigPtr config) {
    if (config->GetStreamingMode() == huestream::STREAMING_MODE_UDP) {
        return std::make_shared<huestream::UdpConnector>();
    }

    return std::make_shared<huestream::DtlsConnector>(huestream::EntropyProviderFactory::Create());
}

template<>
huestream::BasicGroupLightControllerPtr huesdk_lib_default_factory<huestream::BasicGroupLightControllerPtr,
    huestream::HttpClientPtr>(
        huestream::HttpClientPtr httpClient) {
    return std::make_shared<huestream::BasicGroupLightController>(httpClient);
}

#endif  // HUESDK_HUESTREAMDEFAULTFACTORIES_H
