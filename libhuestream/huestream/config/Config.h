/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
/** @file */

#ifndef HUESTREAM_CONFIG_CONFIG_H_
#define HUESTREAM_CONFIG_CONFIG_H_

#include <huestream/common/data/BridgeSettings.h>
#include <huestream/stream/StreamSettings.h>
#include <huestream/config/AppSettings.h>

#include <string>
#include <memory>

namespace huestream {

    /**
     available streaming modes
     */
    typedef enum {
        STREAMING_MODE_DTLS = 0,    ///< default secure streaming mode used for actual bridges
        STREAMING_MODE_UDP,         ///< unsecure streaming mode only used for simulator or debugging
    } StreamingMode;

    /**
     configuration settings for the library
     */
    class Config {
    public:
        Config(const std::string &name,
                  const std::string &platform,
                  const std::string &language = "en",
                  const std::string &region = "US");

        ~Config();

    /**
     configure bridge settings
     @note mainly internal use
     */
    PROP_DEFINE(Config, BridgeSettingsPtr, bridgeSettings, BridgeSettings);

    /**
     configure the streaming protocol
     */
    PROP_DEFINE(Config, StreamSettingsPtr, streamSettings, StreamSettings);

    /**
     configure application settings
     */
    PROP_DEFINE(Config, AppSettingsPtr, appSettings, AppSettings);

    /**
     configure streaming mode
     */
    PROP_DEFINE(Config, StreamingMode, streamingMode, StreamingMode);
    };
    typedef std::shared_ptr<Config> ConfigPtr;

}  // namespace huestream

#endif  // HUESTREAM_CONFIG_CONFIG_H_
