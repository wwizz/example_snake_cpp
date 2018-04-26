/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/connect/BasicGroupLightController.h>
#include <libjson/libjson.h>

#include <math.h>
#include <string>
#include <memory>
#include <sstream>

namespace huestream {

        BasicGroupLightController::BasicGroupLightController(HttpClientPtr http) : _http(http), _bridge(nullptr) {
        }

        std::map<BasicGroupLightController::LightPreset, std::tuple<double, double, double>> BasicGroupLightController::_presetSettingsMap = {
            { LIGHT_PRESET_RELAX,       std::make_tuple(0.567, 0.5018, 0.4152) },
            { LIGHT_PRESET_READ,        std::make_tuple(1.0,   0.4450, 0.4067) },
            { LIGHT_PRESET_CONCENTRATE, std::make_tuple(1.0,   0.3690, 0.3719) },
            { LIGHT_PRESET_ENERGIZE,    std::make_tuple(1.0,   0.3143, 0.3301) }
        };
        
        void BasicGroupLightController::SetActiveBridge(BridgePtr bridge) {
            std::unique_lock<std::mutex> lk(_mutex);
            _bridge = bridge;
        }
            
        void BasicGroupLightController::SetOn(bool on) {
            auto url = getBridgeUrl();
            if (url.empty()) {
                return;
            }

            JSONNode actionNode;
            actionNode.push_back(JSONNode("on", on));
            
            httpPut(url, actionNode);
        }

        void BasicGroupLightController::SetBrightness(double brightness) {
            auto url = getBridgeUrl();
            if (url.empty()) {
                return;
            }

            auto bri = static_cast<int>(round(brightness*254));
            JSONNode actionNode;
            actionNode.push_back(JSONNode("bri", bri));
            
            httpPut(url, actionNode);
        }

        void BasicGroupLightController::SetColor(double x, double y) {
            auto url = getBridgeUrl();
            if (url.empty()) {
                return;
            }

            JSONNode xyNode(JSON_ARRAY);
            xyNode.push_back(JSONNode("", x));
            xyNode.push_back(JSONNode("", y));
            xyNode.set_name("xy");
            JSONNode actionNode;
            actionNode.push_back(xyNode);
            
            httpPut(url, actionNode);
        }

        void BasicGroupLightController::SetPreset(LightPreset preset, bool excludeLightsWhichAreOff) {
            auto i = _presetSettingsMap.find(preset);
            if (i == _presetSettingsMap.end()) {
                return;
            }
            auto settings = i->second;
            auto bri = std::get<0>(settings);
            auto x = std::get<1>(settings);
            auto y = std::get<2>(settings);

            SetPreset(bri, x, y, excludeLightsWhichAreOff);
        }

        void BasicGroupLightController::SetPreset(double brightness, double x, double y, bool excludeLightsWhichAreOff) {
            auto url = getBridgeUrl();
            if (url.empty()) {
                return;
            }

            auto bri = static_cast<int>(round(brightness * 254));
            JSONNode actionNode;
            if (!excludeLightsWhichAreOff) {
                actionNode.push_back(JSONNode("on", true));
            }
            actionNode.push_back(JSONNode("bri", bri));
            JSONNode xyNode(JSON_ARRAY);
            xyNode.push_back(JSONNode("", x));
            xyNode.push_back(JSONNode("", y));
            xyNode.set_name("xy");
            actionNode.push_back(xyNode);
            
            httpPut(url, actionNode);
        }
        
        void BasicGroupLightController::SetScene(const std::string &sceneTag) {
            auto url = getBridgeUrl();
            if (url.empty()) {
                return;
            }

            JSONNode actionNode;
            actionNode.push_back(JSONNode("scene", sceneTag));

            httpPut(url, actionNode);
        }

        std::string BasicGroupLightController::getBridgeUrl() {
            std::unique_lock<std::mutex> lk(_mutex);
            std::string url;
            if (_bridge != nullptr && _bridge->IsConnectable() && _bridge->IsValidGroupSelected()) {
                url = _bridge->GetSelectedGroupActionUrl();
            }
            return url;
        }

        void BasicGroupLightController::httpPut(const std::string & url, const JSONNode & actionNode) {
            _request = _http->CreateHttpRequest(url);
            _request->set_verify_ssl(false);
            auto body = actionNode.write();
            _request->do_put(body, [&](const support::HttpRequestError &error, const support::IHttpResponse &response) {});
        }

}  // namespace huestream
