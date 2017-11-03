/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/connect/Authenticator.h>
#include <huestream/config/Config.h>

#include <future>
#include <string>
#include <memory>
#include <sstream>

namespace huestream {


#define CLIP_ERROR_TYPE_PARAMETER_NOT_AVAILABLE 6

        void Authenticator::Authenticate(BridgePtr bridge, AppSettingsPtr appSettings, AutenticateCallbackHandler cb) {
            std::string url = bridge->GetApiRootUrl();
            std::string data;

            // Only request client key if either Api Version is valid or Api Version is unknown
            // If the Api Version is unknown and turns out not to support client key generation then we'll set the Api Version to invalid at error reception
            bool requestClientKey = (bridge->IsValidApiVersion() || bridge->GetApiversion().empty());
            auto deviceType = appSettings->GetName() + "#" + appSettings->GetPlatform();
            if (requestClientKey) {
                data = "{\"devicetype\": \"" + deviceType + "\", \"generateclientkey\": true\n}";
            } else {
                data = "{\"devicetype\": \"" + deviceType + "\"}";
            }

            auto req = std::make_shared<HttpRequestInfo>(HTTP_REQUEST_POST, url, data);
            _http->Execute(req);

            if (!req->GetSuccess()) {
                bridge->SetIsValidIp(false);
            } else {
                auto response = req->GetResponse();
                if (!libjson::is_valid(response)) {
                    bridge->SetIsValidIp(false);
                } else {
                    bridge->SetIsAuthorized(false);
                    JSONNode root = libjson::parse(response);
                    ParseCredentials(root, bridge);
                    if (requestClientKey) {
                        CheckClientkeyAvailable(root, bridge);
                    }
                }
            }

            cb(bridge);
        }

        void Authenticator::Abort() {}

        Authenticator::Authenticator(HttpClientPtr http) : _http(http) {
        }

        void Authenticator::ParseCredentials(JSONNode root, BridgePtr bridge) {
            for (auto it = root.begin(); it != root.end(); ++it) {
                auto entry = it->find("success");
                if (entry != it->end()) {
                    bridge->SetIsAuthorized(true);
                    bridge->SetUser((*entry)["username"].as_string());

                    auto i = entry->find("clientkey");
                    if (i != entry->end()) {
                        bridge->SetClientKey(i->as_string());
                    }

                    break;
                }
            }
        }

        void Authenticator::CheckClientkeyAvailable(JSONNode root, BridgePtr bridge) {
            for (auto it = root.begin(); it != root.end(); ++it) {
                auto entry = it->find("error");
                if (entry != it->end()) {
                    auto i = entry->find("type");
                    if (i != entry->end()) {
                        if (i->as_int() == CLIP_ERROR_TYPE_PARAMETER_NOT_AVAILABLE) {
                            bridge->SetApiversion("0.0.0");
                        }
                    }
                    break;
                }
            }
        }

}  // namespace huestream
