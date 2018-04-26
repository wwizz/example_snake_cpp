/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_HUESTREAMFACTORIES_H_
#define HUESTREAM_HUESTREAMFACTORIES_H_

#include <edtls/wrapper/mbedtls/EntropyProviderBase.h>

#include <memory>
#include <string>

#include "support/util/Factory.h"
#include "support/util/MakeUnique.h"
#include "huestream/stream/DtlsEntropyProvider.h"
#include "huestream/connect/IBridgeStorageAccessor.h"
#include "huestream/connect/BridgeFileStorageAccessor.h"
#include "huestream/connect/BasicGroupLightController.h"
#include "huestream/effect/Mixer.h"
#include "huestream/common/language/DummyTranslator.h"
#include "huestream/IHueStream.h"

using BridgeStorageAccessorFactory
    = support::Factory<huestream::IBridgeStorageAccessor, const std::string&, huestream::BridgeSettingsPtr>;

using MixerFactory = support::Factory<huestream::IMixer>;
using MessageTranslatorFactory = support::Factory<huestream::IMessageTranslator, std::string>;
using EntropyProviderFactory = support::Factory<EntropyProviderBase>;
using ConnectFactory = support::Factory<huestream::IConnect, huestream::HttpClientPtr,
        huestream::MessageDispatcherPtr, huestream::BridgeSettingsPtr,
        huestream::AppSettingsPtr, huestream::StreamPtr, huestream::BridgeStorageAccessorPtr>;

using HueStreamFactory = support::Factory<huestream::IStream, huestream::StreamSettingsPtr, huestream::AppSettingsPtr,
        huestream::TimeManagerPtr, huestream::ConnectorPtr>;
using HttpClientFactory = support::Factory<huestream::IHttpClient>;
using ConnectionMonitorFactory
    = support::Factory<huestream::IConnectionMonitor, huestream::HttpClientPtr, huestream::AppSettingsPtr>;

using MixerFactory = support::Factory<huestream::IMixer>;
using TimeManagerFactory = support::Factory<huestream::ITimeManager>;
using MessageDispatcherFactory = support::Factory<huestream::IMessageDispatcher>;
using ConnectorFactory = support::Factory<huestream::IConnector, huestream::ConfigPtr>;
using GroupControllerFactory = support::Factory<huestream::IBasicGroupLightController, huestream::HttpClientPtr>;

template<>
std::unique_ptr<EntropyProviderBase> huesdk_lib_default_factory<EntropyProviderBase>();

template<>
std::unique_ptr<huestream::IBridgeStorageAccessor> huesdk_lib_default_factory<huestream::IBridgeStorageAccessor,
                                                                const std::string &,
                                                                huestream::BridgeSettingsPtr>(const std::string &fileName,
                                                                                           huestream::BridgeSettingsPtr hueSettings);


template<>
std::unique_ptr<huestream::IMixer> huesdk_lib_default_factory<huestream::IMixer>();

template<>
std::unique_ptr<huestream::IMessageTranslator> huesdk_lib_default_factory<huestream::IMessageTranslator, std::string>(std::string language);

template<>
std::unique_ptr<huestream::IConnect> huesdk_lib_default_factory<huestream::IConnect,
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
std::unique_ptr<huestream::IStream> huesdk_lib_default_factory<huestream::IStream,
                                                huestream::StreamSettingsPtr,
                                                huestream::AppSettingsPtr,
                                                huestream::TimeManagerPtr,
                                                huestream::ConnectorPtr>(
    huestream::StreamSettingsPtr settings,
    huestream::AppSettingsPtr appSettings,
    huestream::TimeManagerPtr timeManager,
    huestream::ConnectorPtr connector);

template<>
std::unique_ptr<huestream::IHttpClient> huesdk_lib_default_factory<huestream::IHttpClient>();

template<>
std::unique_ptr<huestream::IConnectionMonitor> huesdk_lib_default_factory<huestream::IConnectionMonitor,
                                                           huestream::HttpClientPtr, huestream::AppSettingsPtr>(
    huestream::HttpClientPtr httpClient, huestream::AppSettingsPtr appSettings);

template<>
std::unique_ptr<huestream::ITimeManager> huesdk_lib_default_factory<huestream::ITimeManager>();

template<>
std::unique_ptr<huestream::IMessageDispatcher> huesdk_lib_default_factory<huestream::IMessageDispatcher>();

template<>
std::unique_ptr<huestream::IConnector> huesdk_lib_default_factory<huestream::IConnector, huestream::ConfigPtr>(
    huestream::ConfigPtr config);

template<>
std::unique_ptr<huestream::IBasicGroupLightController> huesdk_lib_default_factory<huestream::IBasicGroupLightController,
    huestream::HttpClientPtr>(huestream::HttpClientPtr httpClient);


#endif  // HUESTREAM_HUESTREAMFACTORIES_H_
