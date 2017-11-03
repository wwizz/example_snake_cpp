/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <memory>
#include <huestream/connect/BasicGroupLightController.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "test/huestream/_mock/MockHttpRequest.h"
#include "test/huestream/_mock/MockHttpClient.h"
#include "network/http/HttpResponse.h"

using ::testing::Invoke;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::_;
using ::testing::Return;

namespace huestream {

    typedef int(*http_exec_function)(huesdk_lib::HttpRequestCallback);

    class TestBasicGroupLightController : public testing::Test {
    public:
        BridgePtr _bridge;
        std::shared_ptr<MockHttpClient> _mockHttpClientPtr;
        std::shared_ptr<MockHttpRequest> _mockHttpRequestPtr;
        std::shared_ptr<BasicGroupLightController> _groupController;
        const std::string actionUrl = "http://192.168.1.15/api/8932746jhb23476/groups/1/action";

        virtual void SetUp() {
            _bridge = std::make_shared<Bridge>(std::make_shared<BridgeSettings>());
            _bridge->SetId("11223344");
            _bridge->SetModelId("BSB002");
            _bridge->SetApiversion("1.22.0");
            _bridge->SetIpAddress("192.168.1.15");
            _bridge->SetIsValidIp(true);
            _bridge->SetUser("8932746jhb23476");
            _bridge->SetIsAuthorized(true);
            _bridge->SetClientKey("nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
            auto group = std::make_shared<Group>();
            group->SetId("1");
            auto groups = std::make_shared<GroupList>();
            groups->push_back(group);
            _bridge->SetGroups(groups);
            ASSERT_TRUE(_bridge->SelectGroup("1"));

            _mockHttpClientPtr = std::make_shared<MockHttpClient>();
            _mockHttpRequestPtr = std::make_shared<MockHttpRequest>("", 0);
            EXPECT_CALL(*_mockHttpClientPtr, CreateHttpRequest(actionUrl,_,_,_,_,_)).WillRepeatedly(
                Return(_mockHttpRequestPtr));
            _groupController = std::make_shared<BasicGroupLightController>(_mockHttpClientPtr);
            _groupController->SetActiveBridge(_bridge);
        }

        virtual void TearDown() {
        }

    };

    TEST_F(TestBasicGroupLightController, DontSetWhenInvalidGroup) {
        _bridge->SetSelectedGroup("30");
        ASSERT_FALSE(_bridge->IsValidGroupSelected());
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(_, _)).Times(0);
        _groupController->SetOn(true);
        _groupController->SetBrightness(0.3);
        _groupController->SetColor(0.8, 0.8);
        _groupController->SetPreset(0.8, 0.5, 0.7);
    }

    TEST_F(TestBasicGroupLightController, SetOn) {
        auto body = "{\"on\":true}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body,_)).Times(1);

        _groupController->SetOn(true);
    }

    TEST_F(TestBasicGroupLightController, SetOff) {
        auto body = "{\"on\":false}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetOn(false);
    }

    TEST_F(TestBasicGroupLightController, SetBri) {
        auto body = "{\"bri\":51}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetBrightness(0.2);
    }

    TEST_F(TestBasicGroupLightController, SetColor) {
        auto body = "{\"xy\":[0.1234,0.5678]}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetColor(0.1234, 0.5678);
    }

    TEST_F(TestBasicGroupLightController, SetPresetDefault) {
        auto body = "{\"on\":true,\"bri\":254,\"xy\":[0.445,0.4067]}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetPreset(BasicGroupLightController::LIGHT_PRESET_READ);
    }

    TEST_F(TestBasicGroupLightController, SetPresetDefaultExcludeOff) {
        auto body = "{\"bri\":254,\"xy\":[0.445,0.4067]}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetPreset(BasicGroupLightController::LIGHT_PRESET_READ, true);
    }

    TEST_F(TestBasicGroupLightController, SetPresetCustom) {
        auto body = "{\"on\":true,\"bri\":127,\"xy\":[0.6,0.7]}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetPreset(0.5, 0.6, 0.7);
    }

    TEST_F(TestBasicGroupLightController, SetPresetCustomExcludeOff) {
        auto body = "{\"bri\":127,\"xy\":[0.6,0.7]}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetPreset(0.5, 0.6, 0.7, true);
    }

    TEST_F(TestBasicGroupLightController, SetScene) {
        auto body = "{\"scene\":\"1234abcd\"}";
        EXPECT_CALL(*_mockHttpRequestPtr, do_put(body, _)).Times(1);

        _groupController->SetScene("1234abcd");
    }

}

