/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/BridgeSettings.h>

namespace huestream {

    PROP_IMPL(BridgeSettings, int, supportedApiVersionMajor, SupportedApiVersionMajor);
    PROP_IMPL(BridgeSettings, int, supportedApiVersionMinor, SupportedApiVersionMinor);
    PROP_IMPL(BridgeSettings, int, supportedApiVersionBuild, SupportedApiVersionBuild);
    PROP_IMPL(BridgeSettings, int, supportedModel, SupportedModel);

    PROP_IMPL(BridgeSettings, int, supportedHttpsApiVersionMajor, SupportedHttpsApiVersionMajor);
    PROP_IMPL(BridgeSettings, int, supportedHttpsApiVersionMinor, SupportedHttpsApiVersionMinor);
    PROP_IMPL(BridgeSettings, int, supportedHttpsApiVersionBuild, SupportedHttpsApiVersionBuild);

    BridgeSettings::BridgeSettings() {
        SetSupportedApiVersionMajor(1);
        SetSupportedApiVersionMinor(22);
        SetSupportedApiVersionBuild(0);
        SetSupportedModel(2);

        SetSupportedHttpsApiVersionMajor(1);
        SetSupportedHttpsApiVersionMinor(24);
        SetSupportedHttpsApiVersionBuild(0);
    }
}  // namespace huestream
