/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/config/AppSettings.h>

#include <string>

namespace huestream {

    PROP_IMPL_BOOL(AppSettings, bool, useForcedActivation, UseForcedActivation);
    PROP_IMPL_BOOL(AppSettings, bool, autoStartAtConnection, AutoStartAtConnection);
    PROP_IMPL_BOOL(AppSettings, bool, useRenderThread, UseRenderThread);
    PROP_IMPL(AppSettings, std::string, name, Name);
    PROP_IMPL(AppSettings, std::string, platform, Platform);
    PROP_IMPL(AppSettings, std::string, language, Language);
    PROP_IMPL(AppSettings, std::string, region, Region);
    PROP_IMPL(AppSettings, std::string, bridgeFileName, BridgeFileName);
    PROP_IMPL(AppSettings, int, monitorIntervalStreamingMs, MonitorIntervalStreamingMs);
    PROP_IMPL(AppSettings, int, monitorIntervalNotStreamingMs, MonitorIntervalNotStreamingMs);

    AppSettings::AppSettings() {
        SetUseForcedActivation(true);
        SetAutoStartAtConnection(true);
        SetUseRenderThread(true);
        SetName("");
        SetPlatform("");
        SetLanguage("en");
        SetRegion("US");
        SetBridgeFileName(BRIDGE_STORAGE_FILENAME);
        SetMonitorIntervalStreamingMs(5000);
        SetMonitorIntervalNotStreamingMs(15000);
    }
}  // namespace huestream
