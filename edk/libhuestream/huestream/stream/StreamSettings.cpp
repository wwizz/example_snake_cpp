/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/stream/StreamSettings.h>

namespace huestream {

    PROP_IMPL(StreamSettings, int, updateFrequency, UpdateFrequency);
    PROP_IMPL(StreamSettings, ColorSpace, streamingColorSpace, StreamingColorSpace);
    PROP_IMPL(StreamSettings, int, streamingPort, StreamingPort);

    StreamSettings::StreamSettings() {
        SetUpdateFrequency(50);
        SetStreamingColorSpace(COLORSPACE_RGB);
        SetStreamingPort(2100);
    }
}  // namespace huestream
