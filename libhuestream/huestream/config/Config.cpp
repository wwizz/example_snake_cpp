/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/config/Config.h>

#include <memory>
#include <string>

using std::make_shared;

namespace huestream {

    Config::Config(const std::string &name,
                              const std::string &platform,
                              const std::string &language,
                              const std::string &region) :
            _bridgeSettings(std::make_shared<BridgeSettings>()),
            _streamSettings(std::make_shared<StreamSettings>()),
            _appSettings(std::make_shared<AppSettings>()),
            _streamingMode(STREAMING_MODE_DTLS) {
        _appSettings->SetName(name);
        _appSettings->SetPlatform(platform);
        _appSettings->SetLanguage(language);
        _appSettings->SetRegion(region);
    }

    Config::~Config() {
        _bridgeSettings = nullptr;
        _streamSettings = nullptr;
        _appSettings = nullptr;
    }

    PROP_IMPL(Config, BridgeSettingsPtr, bridgeSettings, BridgeSettings);
    PROP_IMPL(Config, StreamSettingsPtr, streamSettings, StreamSettings);
    PROP_IMPL(Config, AppSettingsPtr, appSettings, AppSettings);
    PROP_IMPL(Config, StreamingMode, streamingMode, StreamingMode);

}  // namespace huestream
