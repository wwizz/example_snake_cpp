/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_IBRIDGEAUTHENTICATOR_H_
#define HUESTREAM_CONNECT_IBRIDGEAUTHENTICATOR_H_

#include <huestream/common/data/Bridge.h>
#include <huestream/config/AppSettings.h>

#include <memory>

namespace huestream {

        typedef std::function<void(BridgePtr)> AutenticateCallbackHandler;

        class IBridgeAuthenticator {
        public:
            virtual ~IBridgeAuthenticator() {}

            virtual void Authenticate(BridgePtr bridge, AppSettingsPtr appSetting, AutenticateCallbackHandler cb) = 0;

        protected:
        };

        typedef std::shared_ptr<IBridgeAuthenticator> BridgeAuthenticatorPtr;

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_IBRIDGEAUTHENTICATOR_H_
