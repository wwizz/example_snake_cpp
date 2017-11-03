/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_BASICGROUPLIGHTCONTROLLER_H_
#define HUESTREAM_CONNECT_BASICGROUPLIGHTCONTROLLER_H_

#include <huestream/common/http/IHttpClient.h>
#include <huestream/connect/IBasicGroupLightController.h>

namespace huestream {

        class BasicGroupLightController : public IBasicGroupLightController {
        public:
            explicit BasicGroupLightController(HttpClientPtr http);

            void SetActiveBridge(BridgePtr bridge) override;
            
            void SetOn(bool on) override;

            void SetBrightness(double bri) override;

            void SetColor(double x, double y) override;

            void SetPreset(LightPreset preset, bool excludeLightsWhichAreOff = false) override;

            void SetPreset(double bri, double x, double y, bool excludeLightsWhichAreOff = false) override;
            
            void SetScene(const std::string &sceneId) override;

        protected:
            static std::map<LightPreset, std::tuple<double, double, double>> _presetSettingsMap;

            HttpClientPtr _http;
            BridgePtr _bridge;
            std::mutex _mutex;

            std::string getBridgeUrl();
            void httpPut(const std::string &url, const JSONNode &actionNode);
        };

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_BASICGROUPLIGHTCONTROLLER_H_
