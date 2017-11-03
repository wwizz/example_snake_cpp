/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_HUESTREAMFACTORIES_H_
#define HUESTREAM_HUESTREAMFACTORIES_H_

#include <util/Factory.h>
#include <huestream/stream/DtlsEntropyProvider.h>
#include <huestream/connect/IBridgeStorageAccessor.h>
#include <huestream/connect/BridgeFileStorageAccessor.h>
#include <huestream/effect/Mixer.h>
#include <huestream/common/language/DummyTranslator.h>
#include <huestream/HueStream.h>

#include <string>


template<>
EntropyProviderBasePtr huesdk_lib_default_factory<EntropyProviderBasePtr>();

template<>
huestream::BridgeStorageAccessorPtr huesdk_lib_default_factory<huestream::BridgeStorageAccessorPtr,
                                                                const std::string &,
                                                                huestream::BridgeSettingsPtr>(const std::string &fileName,
                                                                                           huestream::BridgeSettingsPtr hueSettings);


template<>
huestream::MixerPtr huesdk_lib_default_factory<huestream::MixerPtr>();

template<>
huestream::MessageTranslatorPtr huesdk_lib_default_factory<huestream::MessageTranslatorPtr, std::string>(std::string language);

template<>
huestream::ConnectPtr huesdk_lib_default_factory<huestream::ConnectPtr,
                                                    huestream::HttpClientPtr,
                                                    huestream::MessageDispatcherPtr,
                                                    huestream::BridgeSettingsPtr,
                                                    huestream::AppSettingsPtr,
                                                    huestream::StreamPtr,
                                                    huestream::BridgeStorageAccessorPtr>(
    huestream::HttpClientPtr,
    huestream::MessageDispatcherPtr,
    huestream::BridgeSettingsPtr,
    huestream::AppSettingsPtr,
    huestream::StreamPtr,
    huestream::BridgeStorageAccessorPtr);

template<>
huestream::StreamPtr huesdk_lib_default_factory<huestream::StreamPtr,
                                                huestream::StreamSettingsPtr,
                                                huestream::AppSettingsPtr,
                                                huestream::TimeManagerPtr,
                                                huestream::ConnectorPtr>(
    huestream::StreamSettingsPtr settings,
    huestream::AppSettingsPtr appSettings,
    huestream::TimeManagerPtr timeManager,
    huestream::ConnectorPtr connector);

template<>
huestream::HttpClientPtr huesdk_lib_default_factory<huestream::HttpClientPtr>();

template<>
huestream::ConnectionMonitorPtr huesdk_lib_default_factory<huestream::ConnectionMonitorPtr,
                                                           huestream::HttpClientPtr, huestream::AppSettingsPtr>(
    huestream::HttpClientPtr httpClient, huestream::AppSettingsPtr appSettings);

template<>
huestream::TimeManagerPtr huesdk_lib_default_factory<huestream::TimeManagerPtr>();

template<>
huestream::MessageDispatcherPtr huesdk_lib_default_factory<huestream::MessageDispatcherPtr>();

template<>
huestream::ConnectorPtr huesdk_lib_default_factory<huestream::ConnectorPtr, huestream::ConfigPtr>(
    huestream::ConfigPtr config);

template<>
huestream::BasicGroupLightControllerPtr huesdk_lib_default_factory<huestream::BasicGroupLightControllerPtr,
    huestream::HttpClientPtr>(huestream::HttpClientPtr httpClient);


#endif  // HUESTREAM_HUESTREAMFACTORIES_H_
