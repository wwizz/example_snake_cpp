/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_COMMON_DATA_BRIDGESETTINGS_H_
#define HUESTREAM_COMMON_DATA_BRIDGESETTINGS_H_

#include <huestream/common/serialize/SerializerHelper.h>

#include <memory>

namespace huestream {

    class BridgeSettings {
    public:
        BridgeSettings();

    PROP_DEFINE(BridgeSettings, int, supportedApiVersionMajor, SupportedApiVersionMajor);
    PROP_DEFINE(BridgeSettings, int, supportedApiVersionMinor, SupportedApiVersionMinor);
    PROP_DEFINE(BridgeSettings, int, supportedApiVersionBuild, SupportedApiVersionBuild);
    PROP_DEFINE(BridgeSettings, int, supportedModel, SupportedModel);
    };

    typedef std::shared_ptr<BridgeSettings> BridgeSettingsPtr;
}  // namespace huestream

#endif  // HUESTREAM_COMMON_DATA_BRIDGESETTINGS_H_
