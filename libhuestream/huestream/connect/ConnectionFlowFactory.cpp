/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/connect/ConnectionFlowFactory.h>

#include <memory>

namespace huestream {

ConnectionFlowFactory::ConnectionFlowFactory(BridgeSettingsPtr bridgeSettings,
                                                           HttpClientPtr http,
                                                           MessageDispatcherPtr messageDispatcher,
                                                           BridgeStorageAccessorPtr storageAccessor) :
    _bridgeSettings(bridgeSettings),
    _http(http),
    _messageDispatcher(messageDispatcher),
    _storageAccessor(storageAccessor) {
}

BridgeSearcherPtr ConnectionFlowFactory::CreateSearcher() {
    return std::make_shared<BridgeSearcher>(_bridgeSettings);
}

BridgeAuthenticatorPtr ConnectionFlowFactory::CreateAuthenticator() {
    return std::make_shared<Authenticator>(_http);
}

MessageDispatcherPtr ConnectionFlowFactory::GetMessageDispatcher() {
    return _messageDispatcher;
}

BridgeStorageAccessorPtr ConnectionFlowFactory::GetStorageAccesser() {
    return _storageAccessor;
}

FullConfigRetrieverPtr ConnectionFlowFactory::CreateFullConfigRetriever(bool useForcedActivation) {
    return std::make_shared<FullConfigRetriever>(_http, useForcedActivation);
}

}  // namespace huestream
