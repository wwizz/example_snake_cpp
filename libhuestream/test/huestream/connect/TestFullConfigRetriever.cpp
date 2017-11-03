/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <memory>
#include <huestream/connect/FullConfigRetriever.h>
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

    class TestFullConfigRetriever : public testing::TestWithParam<http_exec_function> {
    public:
        BridgePtr _bridge;
        std::shared_ptr<MockHttpClient> _mockHttpClientPtr;
        std::shared_ptr<MockHttpRequest> _mockHttpRequestPtr;
        std::shared_ptr<FullConfigRetriever> _fullConfigRetriever;
        const std::string fullConfigUrl = "http://192.168.1.15/api/8932746jhb23476/";

        virtual void SetUp() {
            _bridge = std::make_shared<Bridge>(std::make_shared<BridgeSettings>());
            _bridge->SetIpAddress("192.168.1.15");
            _bridge->SetIsValidIp(true);
            _bridge->SetUser("8932746jhb23476");
            _bridge->SetIsAuthorized(true);
            _bridge->SetSelectedGroup("1");

            _mockHttpClientPtr = std::make_shared<MockHttpClient>();
            _mockHttpRequestPtr = std::make_shared<MockHttpRequest>("", 0);
            EXPECT_CALL(*_mockHttpClientPtr, CreateHttpRequest(fullConfigUrl,_,_,_,_,_)).WillRepeatedly(
                Return(_mockHttpRequestPtr));
            _fullConfigRetriever = std::make_shared<FullConfigRetriever>(_mockHttpClientPtr);
        }

        void expect_retrieve_succesful(OperationResult result, BridgePtr bridge) {
            ASSERT_THAT(result, Eq(OPERATION_SUCCESS));

            EXPECT_THAT(bridge->GetIpAddress(), Eq("192.168.1.15"));
            EXPECT_THAT(bridge->IsValidIp(), Eq(true));
            EXPECT_THAT(bridge->GetUser(), Eq("8932746jhb23476"));
            EXPECT_THAT(bridge->IsAuthorized(), Eq(true));

            EXPECT_THAT(bridge->GetId(), Eq("0017DEADBEEF"));
            EXPECT_THAT(bridge->GetApiversion(), Eq("1.15.0"));
            EXPECT_THAT(bridge->GetModelId(), Eq("BSB002"));
            EXPECT_THAT(bridge->GetName(), Eq("Philips hue unittest"));
            EXPECT_THAT(bridge->GetMaxNoStreamingSessions(), Eq(1));
        }

        void expect_group1_parsed_correctly(GroupPtr group) const {
            EXPECT_THAT(group->GetId(), Eq("1"));
            EXPECT_THAT(group->GetName(), Eq("XBOX"));
            EXPECT_THAT(group->GetClassType(), Eq(GROUPCLASS_TV));
            ASSERT_THAT(group->GetLights()->size(), Eq(3));
            EXPECT_THAT(group->GetLights()->at(0)->GetId(), Eq("1"));
            EXPECT_THAT(group->GetLights()->at(0)->GetPosition().GetX(), DoubleEq(-1));
            EXPECT_THAT(group->GetLights()->at(0)->GetPosition().GetY(), DoubleEq(1));
            EXPECT_THAT(group->GetLights()->at(0)->GetName(), Eq("Hue goooo"));
            EXPECT_THAT(group->GetLights()->at(0)->GetModel(), Eq("LLC020"));
            EXPECT_THAT(group->GetLights()->at(0)->Reachable(), Eq(true));
            EXPECT_THAT(group->GetLights()->at(1)->GetId(), Eq("2"));
            EXPECT_THAT(group->GetLights()->at(1)->GetPosition().GetX(), DoubleEq(0.5));
            EXPECT_THAT(group->GetLights()->at(1)->GetPosition().GetY(), DoubleEq(-0.99));
            EXPECT_THAT(group->GetLights()->at(1)->GetName(), Eq("Strip"));
            EXPECT_THAT(group->GetLights()->at(1)->GetModel(), Eq("LST002"));
            EXPECT_THAT(group->GetLights()->at(1)->Reachable(), Eq(true));
            EXPECT_THAT(group->GetLights()->at(2)->GetId(), Eq("3"));
            EXPECT_THAT(group->GetLights()->at(2)->GetPosition().GetX(), DoubleEq(0.1));
            EXPECT_THAT(group->GetLights()->at(2)->GetPosition().GetY(), DoubleEq(-0.1));
            EXPECT_THAT(group->GetLights()->at(2)->GetName(), Eq("Hue go 3"));
            EXPECT_THAT(group->GetLights()->at(2)->GetModel(), Eq("LLC020"));
            EXPECT_THAT(group->GetLights()->at(2)->Reachable(), Eq(false));
            EXPECT_THAT(group->Active(), Eq(false));
            EXPECT_THAT(group->OnState(), Eq(true));
            EXPECT_THAT(group->GetBrightnessState(), DoubleEq(0.5));
            EXPECT_THAT(group->GetScenes()->size(), Eq(0));
        }

        void expect_group3_parsed_correctly(GroupPtr group) const {
            EXPECT_THAT(group->GetId(), Eq("3"));
            EXPECT_THAT(group->GetName(), Eq("Mancave"));
            EXPECT_THAT(group->GetClassType(), Eq(GROUPCLASS_FREE));
            EXPECT_THAT(group->GetLights()->size(), Eq(1));
            ASSERT_THAT(group->GetLights()->at(0)->GetId(), Eq("4"));
            EXPECT_THAT(group->GetLights()->at(0)->GetPosition().GetX(), DoubleEq(0.1));
            EXPECT_THAT(group->GetLights()->at(0)->GetPosition().GetY(), DoubleEq(0.2));
            EXPECT_THAT(group->GetLights()->at(0)->GetName(), Eq("Hue go 4"));
            EXPECT_THAT(group->GetLights()->at(0)->GetModel(), Eq("LLC020"));
            EXPECT_THAT(group->GetLights()->at(0)->Reachable(), Eq(true));
            EXPECT_THAT(group->Active(), Eq(true));
            EXPECT_THAT(group->GetOwner(), Eq("4LQDOrk3pBfgnjw8PXvwyLVRbYcJ8DQDGQMbg0EA"));
            EXPECT_THAT(group->GetFriendlyOwnerName(), Eq("AnApp#APlatform"));
            EXPECT_THAT(group->OnState(), Eq(false));
            EXPECT_THAT(group->GetBrightnessState(), DoubleEq(0.0));
            ASSERT_THAT(group->GetScenes()->size(), Eq(2));
            EXPECT_THAT(group->GetScenes()->at(0)->GetName(), Eq("Tropical twilight"));
            EXPECT_THAT(group->GetScenes()->at(0)->GetTag(), Eq("xXxDktrZIXyTtVw"));
            EXPECT_THAT(group->GetScenes()->at(0)->GetAppData(), Eq("BNRqq_r03_d16"));
            EXPECT_THAT(group->GetScenes()->at(1)->GetName(), Eq("Savanna sunset"));
            EXPECT_THAT(group->GetScenes()->at(1)->GetTag(), Eq("Gf7-hxlLyW5lG9l"));
            EXPECT_THAT(group->GetScenes()->at(1)->GetAppData(), Eq("HECxM_r03_d15"));
        }

        void expect_group4_parsed_correctly(GroupPtr group) const {
            EXPECT_THAT(group->GetId(), Eq("4"));
            EXPECT_THAT(group->GetName(), Eq("Mancave"));
            EXPECT_THAT(group->GetClassType(), Eq(GROUPCLASS_OTHER));
            EXPECT_THAT(group->GetLights()->size(), Eq(1));
            ASSERT_THAT(group->GetLights()->at(0)->GetId(), Eq("4"));
            EXPECT_THAT(group->GetLights()->at(0)->GetPosition().GetX(), DoubleEq(-0.1));
            EXPECT_THAT(group->GetLights()->at(0)->GetPosition().GetY(), DoubleEq(-0.2));
            EXPECT_THAT(group->GetLights()->at(0)->GetName(), Eq("Hue go 4"));
            EXPECT_THAT(group->GetLights()->at(0)->GetModel(), Eq("LLC020"));
            EXPECT_THAT(group->GetLights()->at(0)->Reachable(), Eq(true));
            EXPECT_THAT(group->Active(), Eq(false));
            EXPECT_THAT(group->OnState(), Eq(true));
            EXPECT_THAT(group->GetScenes()->size(), Eq(0));
        }

        virtual void TearDown() {
        }

    };

    static int groups = 0;

    static int SetHttpFullConfigResponseSuccess(huesdk_lib::HttpRequestCallback callback) {
        const std::string eGroupTV = "\"1\":{\"name\":\"XBOX\",\"type\":\"Entertainment\",\"class\":\"TV\","
                "\"lights\":[\"1\",\"2\",\"3\"],\"locations\":{\"1\":[-1,1,0],\"2\":[0.5,-0.99,0],\"3\":[0.1,-0.1,0.2]},"
                "\"stream\":{\"proxymode\":\"auto\",\"proxynode\":\"/lights/2\",\"active\":false,\"owner\":null},"
                "\"action\":{\"on\":true},\"state\":{\"all_on\":true,\"any_on\":true}}";
        const std::string eGroupFree = "\"3\":{\"name\":\"Mancave\",\"type\":\"Entertainment\",\"class\":\"Free\","
                "\"lights\":[4],\"locations\":{\"4\":[0.1,0.2,0.3]},"
                "\"stream\":{\"proxymode\":\"auto\",\"proxynode\":\"/lights/4\",\"active\":true,\"owner\":\"4LQDOrk3pBfgnjw8PXvwyLVRbYcJ8DQDGQMbg0EA\"},"
                "\"action\":{\"on\":false},\"state\":{\"all_on\":false,\"any_on\":false}}";
        const std::string eGroupOther = "\"4\":{\"name\":\"Mancave\",\"type\":\"Entertainment\",\"class\":\"Other\","
                "\"lights\":[\"4\"],\"locations\":{\"4\":[-0.1,-0.2,-0.3]},"
                "\"stream\":{\"proxymode\":\"manual\",\"proxynode\":\"/bridge\",\"active\":false,\"owner\":null},"
                "\"action\":{\"on\":false},\"state\":{\"all_on\":false,\"any_on\":true}}";
        const std::string lightGroup = "\"2\":{\"name\":\"Hall\",\"type\":\"Lightgroup\",\"lights\":[\"1\",\"2\"],\"action\":{\"on\":true}}";
        const std::string config =
                "{\"name\":\"Philips hue unittest\",\"zigbeechannel\":11,\"bridgeid\":\"0017DEADBEEF\","
                        "\"mac\":\"00:17:DE:ED:BE:EF\",\"dhcp\":true,\"ipaddress\":\"127.0.0.1\",\"netmask\":\"255.255.255.0\","
                        "\"gateway\":\"192.168.1.100\",\"UTC\":\"2016-09-28T12:14:29\","
                        "\"localtime\":\"none\",\"timezone\":\"none\",\"modelid\":\"BSB002\",\"swversion\":\"01035891\","
                        "\"apiversion\":\"1.15.0\",\"swupdate\":{\"updatestate\":0,\"checkforupdate\":false,"
                        "\"devicetypes\":{\"bridge\":false,\"lights\":[],\"sensors\":[]},\"url\":\"\",\"text\":\"\",\"notify\":false},"
                        "\"linkbutton\":false,\"portalservices\":false,\"portalconnection\":\"connected\","
                        "\"portalstate\":{\"signedon\":true,\"incoming\":false,\"outgoing\":true,\"communication\":\"disconnected\","
                        "\"debug-eventing\":\"disconnected\"},\"factorynew\":false,\"replacesbridgeid\":null,"
                        "\"backup\":{\"status\":\"idle\",\"errorcode\":0},"
                        "\"whitelist\":{\"4LQDOrk3pBfgnjw8PXvwyLVRbYcJ8DQDGQMbg0EA\": {\"last use date\":\"2016-10-28T07:55:10\",\"create date\":\"2016-10-28T07:55:10\",\"name\":\"AnApp#APlatform\"},"
                        "\"CkS9yqx6vmiDFd22QYlkKsYnkGVk2YBC95nVdiLw\":{\"last use date\":\"2017-01-30T08:48:16\",\"create date\":\"2017-01-23T13:10:47\",\"name\":\"AGame#APlatform\"}}}";
        const std::string lights = "{\"1\":{\"state\":{\"on\":true,\"bri\":120,\"hue\":0,\"sat\":0,\"effect\":\"none\",\"xy\":[0.0000,0.0000],"
                        "\"ct\":0,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":true},\"swupdate\":{\"state\":\"transferring\",\"lastinstall\":null},"
                        "\"type\":\"Extended color light\",\"name\":\"Hue goooo\",\"modelid\":\"LLC020\",\"manufacturername\":\"Philips\","
                        "\"uniqueid\":\"00:17:88:01:01:15:4b:f5-0b\",\"swversion\":\"5.90.0.19950\"},\"2\":{\"state\":{\"on\":true,\"bri\":134,\"hue\":0,\"sat\":0,\"effect\":\"none\","
                        "\"xy\":[0.3804,0.3768],\"ct\":247,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":true},\"swupdate\":{\"state\":\"noupdates\",\"lastinstall\":\"2017-06-19T07:57:19\"},"
                        "\"type\":\"Extended color light\",\"name\":\"Strip\",\"modelid\":\"LST002\",\"manufacturername\":\"Philips\",\"uniqueid\":\"00:17:88:01:01:14:f0:fb-0b\",\"swversion\":\"5.90.0.19957\"},"
                        "\"3\":{\"state\":{\"on\":true,\"bri\":254,\"hue\":0,\"sat\":0,\"effect\":\"none\",\"xy\":[0.3804,0.3768],\"ct\":247,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":false},"
                        "\"swupdate\":{\"state\":\"noupdates\",\"lastinstall\":\"2017-06-19T07:57:13\"},\"type\":\"Extended color light\",\"name\":\"Hue go 3\",\"modelid\":\"LLC020\","
                        "\"manufacturername\":\"Philips\",\"uniqueid\":\"00:17:88:01:01:16:0a:db-0b\",\"swversion\":\"5.90.0.19957\"},\"4\":{\"state\":{\"on\":false,\"bri\":254,\"hue\":0,\"sat\":0,\"effect\":\"none\","
                        "\"xy\":[0.3804,0.3768],\"ct\":247,\"alert\":\"none\",\"colormode\":\"hs\",\"reachable\":true},\"swupdate\":{\"state\":\"noupdates\",\"lastinstall\":\"2017-06-19T07:57:07\"},"
                        "\"type\":\"Extended color light\",\"name\":\"Hue go 4\",\"modelid\":\"LLC020\",\"manufacturername\":\"Philips\",\"uniqueid\":\"00:17:88:01:01:15:4b:ef-0b\",\"swversion\":\"5.90.0.19957\"}}";
        const std::string capabilitiesActiveStreaming = "{\"streaming\":{\"available\":0,\"total\":1}}";
        const std::string capabilitiesNotActiveStreaming = "{\"streaming\":{\"available\":1,\"total\":1}}";
        const std::string scenes = "{\"xXxDktrZIXyTtVw\":{\"name\":\"Tropical twilight\",\"lights\":[\"4\"],\"owner\":\"M0CBl0UTwWg8QFwlLYhOrFyKs6jBINWRBuKPk3Kc\",\"recycle\":false,\"locked\":false,"
                        "\"appdata\":{\"version\":1,\"data\":\"BNRqq_r03_d16\"},\"picture\":\"\",\"lastupdated\":\"2017-11-14T19:23:52\",\"version\":2},\"Gf7-hxlLyW5lG9l\":{\"name\":\"Savanna sunset\","
                        "\"lights\":[\"4\"],\"owner\":\"M0CBl0UTwWg8QFwlLYhOrFyKs6jBINWRBuKPk3Kc\",\"recycle\":false,\"locked\":false,\"appdata\":{\"version\":1,\"data\":\"HECxM_r03_d15\"},"
                        "\"picture\":\"\",\"lastupdated\":\"2017-11-14T19:23:51\",\"version\":2},\"Q2SoDz-rQnXkwJO\":{\"name\":\"Arctic aurora\",\"lights\":[\"1\",\"2\",\"11\",\"12\",\"16\",\"17\",\"18\",\"19\",\"20\",\"21\",\"22\"],"
                        "\"owner\":\"M0CBl0UTwWg8QFwlLYhOrFyKs6jBINWRBuKPk3Kc\",\"recycle\":false,\"locked\":false,\"appdata\":{\"version\":1,\"data\":\"2rPOL_r07_d17\"},\"picture\":\"\",\"lastupdated\":\"2017-11-14T19:23:52\",\"version\":2}}";

        std::string fullConfigResponse;
        if (groups == 0) {
            fullConfigResponse = "{\"capabilities\":" + capabilitiesNotActiveStreaming + ",\"config\":" + config + 
                ",\"groups\":{},\"lights\":{},\"scenes\":{}}";
        } else if (groups == 1) {
            fullConfigResponse = "{\"capabilities\":" + capabilitiesNotActiveStreaming + ",\"config\":" + config + 
                ",\"groups\":{" + eGroupTV + "," + lightGroup + "},\"lights\":" + lights + ",\"scenes\":" + scenes + "}";
        } else if (groups == 2) {
            fullConfigResponse = "{\"capabilities\":" + capabilitiesActiveStreaming + ",\"config\":" + config +
                ",\"groups\":{" + lightGroup + "," + eGroupFree + "," + eGroupOther + "},\"lights\":" + lights + ",\"scenes\":" + scenes + "}";
        } else {
            return huesdk_lib::HTTP_REQUEST_STATUS_FAILED;
        }

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(fullConfigResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseNotAuthorized(huesdk_lib::HttpRequestCallback callback) {
        const std::string notAuthenticatedResponse = "[{\"error\":{\"type\":1,\"address\":\"/\",\"description\":\"unauthorized user\"}}]";

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(notAuthenticatedResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseUnknownError(huesdk_lib::HttpRequestCallback callback) {
        const std::string unknownErrorResponse = "[{\"error\":{\"type\":4,\"address\":\"\",\"description\":\"method, GET, not available for resource\"}}]";

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(unknownErrorResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseNoConfigResource(huesdk_lib::HttpRequestCallback callback) {
        const std::string noConfigResponse = "{\"lights\":{},\"groups\":{}}";

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(noConfigResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseNoGroupsResource(huesdk_lib::HttpRequestCallback callback) {
        const std::string noGroupsResponse = "{\"capabilities\":{},\"config\":{},\"lights\":{}}";

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(noGroupsResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseNoLightsResource(huesdk_lib::HttpRequestCallback callback) {
        const std::string noLightsResponse = "{\"capabilities\":{},\"config\":{},\"groups\":{}}";

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(noLightsResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseNoCapabilitiesResource(huesdk_lib::HttpRequestCallback callback) {
        const std::string noCapabilitiesResponse = "{\"config\":{},\"groups\":{},\"lights\":{}}";

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(noCapabilitiesResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseGroupCorrupted(huesdk_lib::HttpRequestCallback callback) {
        const std::string eGroupCorrupted = "\"3\":{\"nameeeeeeeeeeee\":\"Mancave\",\"type\":\"Entertainment\",\"class\":\"TV\","
                "\"lights\":[\"4\"],\"looooooooocations\":{\"4\":[0.1,0.2,0.3]},\"action\":{\"on\":false}}";
        const std::string config =
                "{\"name\":\"Philips hue unittest\",\"zigbeechannel\":11,\"bridgeid\":\"0017DEADBEEF\","
                        "\"mac\":\"00:17:DE:ED:BE:EF\",\"dhcp\":true,\"ipaddress\":\"127.0.0.1\",\"netmask\":\"255.255.255.0\","
                        "\"gateway\":\"192.168.1.100\",\"UTC\":\"2016-09-28T12:14:29\","
                        "\"localtime\":\"none\",\"timezone\":\"none\",\"modelid\":\"BSB002\",\"swversion\":\"01035891\","
                        "\"apiversion\":\"1.15.0\",\"swupdate\":{\"updatestate\":0,\"checkforupdate\":false,"
                        "\"devicetypes\":{\"bridge\":false,\"lights\":[],\"sensors\":[]},\"url\":\"\",\"text\":\"\",\"notify\":false},"
                        "\"linkbutton\":false,\"portalservices\":false,\"portalconnection\":\"connected\","
                        "\"portalstate\":{\"signedon\":true,\"incoming\":false,\"outgoing\":true,\"communication\":\"disconnected\","
                        "\"debug-eventing\":\"disconnected\"},\"factorynew\":false,\"replacesbridgeid\":null,"
                        "\"backup\":{\"status\":\"idle\",\"errorcode\":0}}";

        std::string groupCorruptedResponse = "{\"config\":" + config + ",\"groups\":{" + eGroupCorrupted + "}}";

        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS);
        response.set_body(groupCorruptedResponse);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    static int SetHttpFullConfigResponseTimeout(huesdk_lib::HttpRequestCallback callback) {
        huesdk_lib::HttpResponse response;
        huesdk_lib::HttpRequestError error;
        error.set_code(huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_TIMEOUT);
        callback(error, response);
        return huesdk_lib::HTTP_REQUEST_STATUS_OK;
    }

    TEST_F(TestFullConfigRetriever, RetrieveSuccessNoGroups) {
        groups = 0;
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
                Invoke(SetHttpFullConfigResponseSuccess));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            expect_retrieve_succesful(result, bridge);

            EXPECT_THAT(bridge->GetGroups()->size(), Eq(0));
            EXPECT_EQ(bridge->IsBusy(), false);
        });
    }

    TEST_F(TestFullConfigRetriever, RetrieveSuccessOneGroup) {
        groups = 1;
        _bridge->SetIsValidIp(false);
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
                Invoke(SetHttpFullConfigResponseSuccess));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            expect_retrieve_succesful(result, bridge);

            ASSERT_THAT(bridge->GetGroups()->size(), Eq(1));

            auto group = bridge->GetGroups()->at(0);
            expect_group1_parsed_correctly(group);
            EXPECT_EQ(bridge->IsBusy(), false);
        });
    }

    TEST_F(TestFullConfigRetriever, RetrieveSuccessTwoGroups) {
        groups = 2;
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
                Invoke(SetHttpFullConfigResponseSuccess));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            expect_retrieve_succesful(result, bridge);

            ASSERT_THAT(bridge->GetGroups()->size(), Eq(2));

            auto group3 = bridge->GetGroups()->at(0);
            expect_group3_parsed_correctly(group3);

            auto group4 = bridge->GetGroups()->at(1);
            expect_group4_parsed_correctly(group4);
            EXPECT_EQ(bridge->IsBusy(), true);
        });
    }

    TEST_F(TestFullConfigRetriever, RetrieveFailButtonNotPressed) {
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
                Invoke(SetHttpFullConfigResponseNotAuthorized));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            ASSERT_THAT(result, Eq(OPERATION_FAILED));

            EXPECT_THAT(bridge->GetIpAddress(), Eq("192.168.1.15"));
            EXPECT_THAT(bridge->IsValidIp(), Eq(true));
            EXPECT_THAT(bridge->IsAuthorized(), Eq(false));
        });
    }

    TEST_F(TestFullConfigRetriever, RetrieveFailUnknownError) {
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
                Invoke(SetHttpFullConfigResponseUnknownError));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            ASSERT_THAT(result, Eq(OPERATION_FAILED));

            EXPECT_THAT(bridge->IsValidIp(), Eq(false));
            EXPECT_THAT(bridge->IsAuthorized(), Eq(true));
        });
    }

    TEST_F(TestFullConfigRetriever, RetrieveFailNoResponse) {
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
                Invoke(SetHttpFullConfigResponseTimeout));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            ASSERT_THAT(result, Eq(OPERATION_FAILED));

            EXPECT_THAT(bridge->IsValidIp(), Eq(false));
            EXPECT_THAT(bridge->IsAuthorized(), Eq(true));
        });
    }

    INSTANTIATE_TEST_CASE_P(RetrieveFailMissingResource,
        TestFullConfigRetriever,
        ::testing::Values(SetHttpFullConfigResponseNoConfigResource, SetHttpFullConfigResponseNoGroupsResource, SetHttpFullConfigResponseNoLightsResource));

    TEST_P(TestFullConfigRetriever, RetrieveFailMissingResource) {
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
            Invoke(GetParam()));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            ASSERT_THAT(result, Eq(OPERATION_FAILED));

            EXPECT_THAT(bridge->IsValidIp(), Eq(false));
            EXPECT_THAT(bridge->IsAuthorized(), Eq(true));
        });
    }

    TEST_F(TestFullConfigRetriever, RetrieveCorruptedGroupHandledGracefully) {
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
                Invoke(SetHttpFullConfigResponseGroupCorrupted));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {

        });
    }

    TEST_P(TestFullConfigRetriever, RetrieveSuccessMissingCapabilitiesResource) {
        EXPECT_CALL(*_mockHttpRequestPtr, do_get(_)).Times(1).WillOnce(
            Invoke(SetHttpFullConfigResponseNoCapabilitiesResource));

        _fullConfigRetriever->Execute(_bridge, [this](OperationResult result, BridgePtr bridge) {
            ASSERT_THAT(result, Eq(OPERATION_SUCCESS));

            EXPECT_THAT(bridge->IsValidIp(), Eq(true));
            EXPECT_THAT(bridge->IsAuthorized(), Eq(true));
        });
    }
}

