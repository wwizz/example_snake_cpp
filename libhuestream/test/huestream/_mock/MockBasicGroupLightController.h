/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef LIBHUESTREAM_TEST_MOCKBASICGROUPCONTROLLER_H_
#define LIBHUESTREAM_TEST_MOCKBASICGROUPCONTROLLER_H_

#include "gmock/gmock.h"
#include "huestream/connect/IBasicGroupLightController.h"

namespace huestream {

    class MockBasicGroupLightController : public IBasicGroupLightController {
    public:
        MockBasicGroupLightController() {}
        MOCK_METHOD1(SetActiveBridge, void(BridgePtr bridge));
        MOCK_METHOD1(SetOn, void(bool on));
        MOCK_METHOD1(SetBrightness, void(double brightness));
        MOCK_METHOD2(SetColor, void(double x, double y));
        MOCK_METHOD2(SetPreset, void(LightPreset preset, bool excludeLightsWhichAreOff));
        MOCK_METHOD4(SetPreset, void(double brightness, double x, double y, bool excludeLightsWhichAreOff));
        MOCK_METHOD1(SetScene, void(const std::string &sceneTag));
    };

}// namespace huestream
#endif     // LIBHUESTREAM_TEST_MOCKBASICGROUPCONTROLLER_H_
