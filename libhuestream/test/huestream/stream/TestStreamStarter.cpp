/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/stream/StreamStarter.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "test/huestream/_mock/MockHttpClient.h"

using ::testing::Return;
using ::testing::_;
using ::testing::Sequence;
using ::testing::Property;
using ::testing::AllOf;
using ::testing::Pointee;
using ::testing::Invoke;
using ::testing::InSequence;

namespace huestream {

    class TestStreamStarter : public testing::Test {
    public:
        BridgePtr _bridge;
        std::shared_ptr<MockHttpClient> _mockHttpPtr;
        std::shared_ptr<StreamStarter> _streamStarter;

        virtual void SetUp() {
            _bridge = CreateBridge();
            _mockHttpPtr = std::make_shared<MockHttpClient>();
            _streamStarter = std::make_shared<StreamStarter>(_bridge, _mockHttpPtr);
        }

        virtual void TearDown() {
        }

        BridgePtr CreateBridge() {
            auto group1 = std::make_shared<Group>();
            group1->SetId("1");
            auto group2 = std::make_shared<Group>();
            group2->SetId("2");
            auto groupList = std::make_shared<GroupList>();
            groupList->push_back(group1);
            groupList->push_back(group2);
            auto bridge = std::make_shared<Bridge>(std::make_shared<BridgeSettings>());
            bridge->SetGroups(groupList);
            bridge->SetSelectedGroup("2");
            bridge->SetIpAddress("192.168.1.15");
            bridge->SetUser("8932746jhb23476");
            bridge->SetIsValidIp(true);
            bridge->SetIsAuthorized(true);
            bridge->SetId("SOMEID");
            bridge->SetModelId("BSB002");
            bridge->SetApiversion("1.22.0");
            bridge->SetClientKey("DD129216F1A50E5D1C0CB356325745F2");
            bridge->SetIsBusy(false);
            return bridge;
        }
    };

    static void SetStreamActiveResponseSuccess(HttpRequestPtr req) {
        req->SetSuccess(true);
        req->SetResponse("[{\"success\":{\"/groups/2/stream/active\":true}}]");
    }

    static void SetStreamActiveResponseErrorNoRepsonse(HttpRequestPtr req) {
        req->SetSuccess(false);
    }

    static void SetStreamActiveResponseErrorCantActivate(HttpRequestPtr req) {
        req->SetSuccess(true);
        req->SetResponse(
            "[{\"error\":{\"type\":307,\"address\":\"/groups/2/stream/active\",\"description\":\"Cannot claim stream ownership\"}}]");
    }

    static void SetStreamActiveResponseErrorNotAuthorized(HttpRequestPtr req) {
        req->SetSuccess(true);
        req->SetResponse(
            "[{\"error\":{\"type\":1,\"address\":\"/groups/2/stream/active\",\"description\":\"unauthorized user\"}}]");
    }

    static void SetStreamActiveResponseErrorInvalidGroup(HttpRequestPtr req) {
        req->SetSuccess(true);
        req->SetResponse(
            "[{\"error\":{\"type\":3,\"address\":\"/groups/2/stream/active\",\"description\":\"resource not available\"}}]");
    }

    static void SetStreamDeactivateResponseSuccess(HttpRequestPtr req) {
        req->SetSuccess(true);
        req->SetResponse("[{\"success\":{\"/groups/2/stream/active\":false}}]");
    }

    TEST_F(TestStreamStarter, StartSuccess) {
        std::string body = "{\"stream\":{\"active\":true}}";
        std::string url = "http://192.168.1.15/api/8932746jhb23476/groups/2";
        EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, body))).Times(1).WillOnce(
                Invoke(SetStreamActiveResponseSuccess));
        auto success = _streamStarter->Start(false);
        ASSERT_TRUE(success);
        ASSERT_FALSE(_bridge->IsBusy());
        ASSERT_TRUE(_bridge->GetGroup()->Active());
        ASSERT_EQ(_bridge->GetGroup()->GetOwner(), _bridge->GetUser());
    }

    TEST_F(TestStreamStarter, StartUnforcedFail) {
        std::string body = "{\"stream\":{\"active\":true}}";
        std::string url = "http://192.168.1.15/api/8932746jhb23476/groups/2";
        EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, body))).Times(1).WillOnce(
                Invoke(SetStreamActiveResponseErrorCantActivate));
        auto success = _streamStarter->Start(false);
        ASSERT_FALSE(success);
        ASSERT_TRUE(_bridge->IsBusy());
    }

    TEST_F(TestStreamStarter, StartForcedSuccess) {
        std::string url = "http://192.168.1.15/api/8932746jhb23476/groups/2";
        std::string bodyStart = "{\"stream\":{\"active\":true}}";
        std::string bodyStop = "{\"stream\":{\"active\":false}}";

        {
            InSequence dummy;
            EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, bodyStart))).Times(1).WillOnce(
                    Invoke(SetStreamActiveResponseErrorCantActivate));
            EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, bodyStop))).Times(1);
            EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, bodyStart))).Times(1).WillOnce(
                    Invoke(SetStreamActiveResponseSuccess));
        }

        auto success = _streamStarter->Start(true);
        ASSERT_TRUE(success);
        ASSERT_FALSE(_bridge->IsBusy());
        ASSERT_TRUE(_bridge->GetGroup()->Active());
    }

    TEST_F(TestStreamStarter, StartFailNoResponse) {
        std::string body = "{\"stream\":{\"active\":true}}";
        std::string url = "http://192.168.1.15/api/8932746jhb23476/groups/2";
        EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, body))).Times(1).WillOnce(
            Invoke(SetStreamActiveResponseErrorNoRepsonse));
        auto success = _streamStarter->Start(true);
        ASSERT_FALSE(success);
        ASSERT_FALSE(_bridge->IsValidIp());
    }

    TEST_F(TestStreamStarter, StartFailNotAuthorized) {
        std::string body = "{\"stream\":{\"active\":true}}";
        std::string url = "http://192.168.1.15/api/8932746jhb23476/groups/2";
        EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, body))).Times(1).WillOnce(
            Invoke(SetStreamActiveResponseErrorNotAuthorized));
        auto success = _streamStarter->Start(false);
        ASSERT_FALSE(success);
        ASSERT_FALSE(_bridge->IsAuthorized());
    }

    TEST_F(TestStreamStarter, StartFailInvalidGroup) {
        std::string body = "{\"stream\":{\"active\":true}}";
        std::string url = "http://192.168.1.15/api/8932746jhb23476/groups/2";
        EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, body))).Times(1).WillOnce(
            Invoke(SetStreamActiveResponseErrorInvalidGroup));
        auto success = _streamStarter->Start(true);
        ASSERT_FALSE(success);
        ASSERT_FALSE(_bridge->IsValidGroupSelected());
    }

    TEST_F(TestStreamStarter, Stop) {
        _bridge->GetGroup()->SetActive(true);
        std::string body = "{\"stream\":{\"active\":false}}";
        std::string url = "http://192.168.1.15/api/8932746jhb23476/groups/2";
        EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_PUT, url, body))).Times(1).WillOnce(
            Invoke(SetStreamDeactivateResponseSuccess));
        _streamStarter->Stop();
        ASSERT_FALSE(_bridge->GetGroup()->Active());
    }


}

