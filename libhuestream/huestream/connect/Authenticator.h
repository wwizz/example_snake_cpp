/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_AUTHENTICATOR_H_
#define HUESTREAM_CONNECT_AUTHENTICATOR_H_

#include <huestream/common/http/IHttpClient.h>
#include <huestream/connect/ConnectionFlow.h>

namespace huestream {

        class Authenticator : public IBridgeAuthenticator {
        public:
            explicit Authenticator(HttpClientPtr http);

            void Authenticate(BridgePtr bridge, AppSettingsPtr appSettings, AutenticateCallbackHandler cb) override;

            void Abort();

        protected:
            HttpClientPtr _http;

            void ParseCredentials(JSONNode root, BridgePtr bridge);

            void CheckClientkeyAvailable(JSONNode root, BridgePtr bridge);
        };

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_AUTHENTICATOR_H_
