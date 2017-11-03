/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/BridgeSettings.h>

namespace huestream {

    PROP_IMPL(BridgeSettings, int, supportedApiVersionMajor, SupportedApiVersionMajor);
    PROP_IMPL(BridgeSettings, int, supportedApiVersionMinor, SupportedApiVersionMinor);
    PROP_IMPL(BridgeSettings, int, supportedApiVersionBuild, SupportedApiVersionBuild);
    PROP_IMPL(BridgeSettings, int, supportedModel, SupportedModel);

    BridgeSettings::BridgeSettings() {
        SetSupportedApiVersionMajor(1);
        SetSupportedApiVersionMinor(22);
        SetSupportedApiVersionBuild(0);
        SetSupportedModel(2);
    }
}  // namespace huestream
