/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
/** @file */

#ifndef HUESTREAM_CONFIG_APPSETTINGS_H_
#define HUESTREAM_CONFIG_APPSETTINGS_H_

#include <huestream/common/serialize/SerializerHelper.h>
#include <huestream/stream/ProtocolSerializer.h>

#include <memory>
#include <string>

namespace huestream {

    /**
     various application configuration settings for the library
     */
    class AppSettings {
    public:
        static constexpr const char* BRIDGE_STORAGE_FILENAME = "bridge.json";

        AppSettings();

    /**
     set whether forced stream activation is used which means the library will override another application already streaming to the same entertainment group
     @note default true, should be false for non content specific applications to give content specific applications priority
     */
    PROP_DEFINE_BOOL(AppSettings, bool, useForcedActivation, UseForcedActivation);

    /**
     set whether the library will automatically start streaming (controlling the lights) right after the bridge has been connected to
     @note default true, should be false when application wants to start controlling lights on another moment than right after connection
     */
    PROP_DEFINE_BOOL(AppSettings, bool, autoStartAtConnection, AutoStartAtConnection);

    /**
     set whether the library runs its own renderthread
     @note default true, should be false when the application wants to control the render loop manually
    */
    PROP_DEFINE_BOOL(AppSettings, bool, useRenderThread, UseRenderThread);

    /**
     set a user friendly name of the application
     @note no default, this is mandatory to set
     */
    PROP_DEFINE(AppSettings, std::string, name, Name);

    /**
     set a user friendly name of the platfrom this application is running on
     @note no default, this is mandatory to set
     */
    PROP_DEFINE(AppSettings, std::string, platform, Platform);

    /**
     set the ISO 639-1 language code used by the library to output user messages in the correct language
     @note default en
     */
    PROP_DEFINE(AppSettings, std::string, language, Language);

    /**
     set the ISO 3166-1 alpha-2 country CODE used by the library to output user messages in the correct language
     @note default US
     */
    PROP_DEFINE(AppSettings, std::string, region, Region);

    /**
     set where the bridge information is stored with full path (absolute or relative) and file name
     @note default bridge.json (next to executable), needs to be a writable location for the platform
     */
    PROP_DEFINE(AppSettings, std::string, bridgeFileName, BridgeFileName);

    /**
     set the bridge monitoring polling interval during streaming
     @note default 5000 (5 seconds)
     */
    PROP_DEFINE(AppSettings, int, monitorIntervalStreamingMs, MonitorIntervalStreamingMs);

    /**
     set the bridge monitoring polling interval when not streaming
     @note default 15000 (15 seconds)
     */
    PROP_DEFINE(AppSettings, int, monitorIntervalNotStreamingMs, MonitorIntervalNotStreamingMs);

    };

    /**
     shared pointer to a huestream::AppSettings object
     */
    typedef std::shared_ptr<AppSettings> AppSettingsPtr;
}  // namespace huestream

#endif  // HUESTREAM_CONFIG_APPSETTINGS_H_
