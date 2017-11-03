/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_CONNECTIONFLOWFACTORY_H_
#define HUESTREAM_CONNECT_CONNECTIONFLOWFACTORY_H_

#include <huestream/connect/IConnectionFlowFactory.h>
#include <huestream/common/http/IHttpClient.h>
#include <huestream/connect/FullConfigRetriever.h>
#include <huestream/connect/BridgeSearcher.h>
#include <huestream/connect/Authenticator.h>

namespace huestream {


class ConnectionFlowFactory : public IConnectionFlowFactory {
 public:
    ConnectionFlowFactory(BridgeSettingsPtr bridgeSettings,
                                 HttpClientPtr http,
                                 MessageDispatcherPtr messageDispatcher,
                                 BridgeStorageAccessorPtr storageAccessor);

    BridgeSearcherPtr CreateSearcher() override;

    BridgeAuthenticatorPtr CreateAuthenticator() override;

    MessageDispatcherPtr GetMessageDispatcher() override;

    BridgeStorageAccessorPtr GetStorageAccesser() override;

    FullConfigRetrieverPtr CreateFullConfigRetriever(bool useForcedActivation = true) override;

 private:
    BridgeSettingsPtr _bridgeSettings;
    HttpClientPtr _http;
    MessageDispatcherPtr _messageDispatcher;
    BridgeStorageAccessorPtr _storageAccessor;
};

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_CONNECTIONFLOWFACTORY_H_
