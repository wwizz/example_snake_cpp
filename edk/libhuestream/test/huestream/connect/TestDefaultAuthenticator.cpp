/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/connect/Authenticator.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "test/huestream/_mock/MockHttpClient.h"

#include <huestream/config/Config.h>

using ::testing::Invoke;
using ::testing::Eq;

namespace huestream {

static std::string s_username;
static std::string s_clientkey;

static void SetHttpAuthenticateResponseSuccessButWithoutClientKey(HttpRequestPtr req) {
    const std::string authenticateResponseSuccess = "[{\"success\":{\"username\": \"" + s_username + "\"}}]";
    req->SetSuccess(true);
    req->SetResponse(authenticateResponseSuccess);
}

static void SetHttpAuthenticateResponseSuccess(HttpRequestPtr req) {
    const std::string authenticateResponseSuccess =
        "[{\"success\":{\"username\": \"" + s_username + "\", \"clientkey\": \"" + s_clientkey + "\"}}]";
    req->SetSuccess(true);
    req->SetResponse(authenticateResponseSuccess);
}


static void SetHttpAuthenticateResponseErrorButtonNotPressed(HttpRequestPtr req) {
    const std::string authenticateResponseError101 =
        "[{\"error\":{\"type\":101,\"address\":\"\",\"description\":\"link button not pressed\"}}]";
    req->SetSuccess(true);
    req->SetResponse(authenticateResponseError101);
}

static void SetHttpAuthenticateResponseTimeout(HttpRequestPtr req) {
    req->SetSuccess(false);
}

static void SetHttpAuthenticateResponseErrorClientkeyNotAvailable(HttpRequestPtr req) {
    const std::string authenticateResponseError6 =
        "[{\"error\":{\"type\":6,\"address\":\"/generateclientkey\",\"description\":\"parameter, generateclientkey, not available\"}}]";
    req->SetSuccess(true);
    req->SetResponse(authenticateResponseError6);
}

class TestDefaultAuthenticator : public testing::Test {
 public:
    BridgePtr _bridge;
    std::shared_ptr<MockHttpClient> _mockHttpPtr;
    std::shared_ptr<Authenticator> _defaultAuthenticator;
    std::string _ip;
    std::string _appName;
    std::string _deviceName;
    const std::string _id = "001788FFFE200646";
    const std::string _apiVersion = "1.22.0";
    const std::string _outdatedApiVersion = "1.18.0";

    virtual void SetUp() {
        _ip = "192.168.1.15";
        s_username = "testUser";
        s_clientkey = "DD129216F1A50E5D1C0CB356325745F2";
        _appName = "AuthUnitTest";
        _deviceName = "PC";
        _mockHttpPtr = std::make_shared<MockHttpClient>();
        _appSettings = std::make_shared<AppSettings>();
        _appSettings->SetAppName(_appName);
        _appSettings->SetDeviceName(_deviceName);
        _bridgeSettings = std::make_shared<BridgeSettings>();

        _bridge = std::make_shared<Bridge>(_bridgeSettings);
        _bridge->SetIpAddress(_ip);
        _bridge->SetIsValidIp(true);
        _bridge->SetId(_id);
        _bridge->SetApiversion(_apiVersion);

        _defaultAuthenticator = std::make_shared<Authenticator>(_mockHttpPtr);
    }

    void checkBasicBridgeAttributes(BridgePtr bridge) const {
        ASSERT_THAT(bridge->GetId(), Eq(_id));
        ASSERT_THAT(bridge->GetIpAddress(), Eq(_ip));
        ASSERT_THAT(bridge->GetUser(), Eq(s_username));
        ASSERT_THAT(bridge->IsValidIp(), Eq(true));
        ASSERT_THAT(bridge->IsAuthorized(), Eq(true));
    }

    const std::string getAuthUrl() {
        const std::string url = "http://" + _ip + "/api/";
        return url;
    }

    const std::string getAuthBodyWithClientKey() {
        const std::string body =
            "{\"devicetype\": \"" + _appName + "#" + _deviceName + "\", \"generateclientkey\": true\n}";
        return body;
    }

    const std::string getAuthBodyWithoutClientKey() {
        const std::string body = "{\"devicetype\": \"" + _appName + "#" + _deviceName + "\"}";
        return body;
    }

    virtual void TearDown() {
    }

    shared_ptr<AppSettings> _appSettings;
    shared_ptr<BridgeSettings> _bridgeSettings;
};


TEST_F(TestDefaultAuthenticator, AuthenticateSuccessWithClientkey) {
    EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(),
                                                        getAuthBodyWithClientKey()))).Times(1).WillOnce(Invoke(SetHttpAuthenticateResponseSuccess));

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        checkBasicBridgeAttributes(b);
        ASSERT_THAT(b->GetClientKey(), Eq(s_clientkey));
        ASSERT_THAT(b->GetApiversion(), Eq(_apiVersion));
    });
}

TEST_F(TestDefaultAuthenticator, ReauthenticateSuccessWithClientkey) {
    EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(),
        getAuthBodyWithClientKey()))).Times(1).WillOnce(Invoke(SetHttpAuthenticateResponseSuccess));

    _bridge->SetUser("oldusername");

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        checkBasicBridgeAttributes(b);
        ASSERT_THAT(b->GetClientKey(), Eq(s_clientkey));
        ASSERT_THAT(b->GetApiversion(), Eq(_apiVersion));
    });
}

TEST_F(TestDefaultAuthenticator, AuthenticateSuccessWithoutClientKey) {
    _bridge->SetApiversion(_outdatedApiVersion);
    EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(),
                                                        getAuthBodyWithoutClientKey()))).Times(1).WillOnce(Invoke(SetHttpAuthenticateResponseSuccessButWithoutClientKey));

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        checkBasicBridgeAttributes(b);
        ASSERT_THAT(b->GetClientKey(), Eq(""));
        ASSERT_THAT(b->GetApiversion(), Eq(_outdatedApiVersion));
    });
}

TEST_F(TestDefaultAuthenticator, AuthenticateSuccessWithClientkeyEmptyBridgeVersion) {
    _bridge->SetApiversion("");
    EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(),
                                                        getAuthBodyWithClientKey()))).Times(1).WillOnce(Invoke(SetHttpAuthenticateResponseSuccess));

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        checkBasicBridgeAttributes(b);
        ASSERT_THAT(b->GetClientKey(), Eq(s_clientkey));
        ASSERT_THAT(b->GetApiversion(), Eq(""));
    });
}

TEST_F(TestDefaultAuthenticator, AuthenticateFailButtonNotPressed) {
    EXPECT_CALL(*_mockHttpPtr,
                Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(), getAuthBodyWithClientKey()))).Times(
        1).WillOnce(Invoke(
        SetHttpAuthenticateResponseErrorButtonNotPressed));

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        ASSERT_THAT(b->GetIpAddress(), Eq(_ip));
        ASSERT_THAT(b->IsValidIp(), Eq(true));
        ASSERT_THAT(b->IsAuthorized(), Eq(false));
        ASSERT_THAT(b->GetId(), Eq(_id));
        ASSERT_THAT(b->GetApiversion(), Eq(_apiVersion));
    });
}

TEST_F(TestDefaultAuthenticator, AuthenticateFailNoResponse) {
    EXPECT_CALL(*_mockHttpPtr,
                Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(), getAuthBodyWithClientKey()))).Times(
        1).WillOnce(Invoke(
        SetHttpAuthenticateResponseTimeout));

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        ASSERT_THAT(b->GetIpAddress(), Eq(_ip));
        ASSERT_THAT(b->IsValidIp(), Eq(false));
        ASSERT_THAT(b->IsAuthorized(), Eq(false));
        ASSERT_THAT(b->GetId(), Eq(_id));
        ASSERT_THAT(b->GetApiversion(), Eq(_apiVersion));
    });
}

TEST_F(TestDefaultAuthenticator, AuthenticateFailClientkeyNotAvailable) {
    _bridge->SetApiversion("");
    EXPECT_CALL(*_mockHttpPtr,
                Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(), getAuthBodyWithClientKey()))).Times(
        1).WillOnce(Invoke(
        SetHttpAuthenticateResponseErrorClientkeyNotAvailable));

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        ASSERT_THAT(b->GetIpAddress(), Eq(_ip));
        ASSERT_THAT(b->IsValidIp(), Eq(true));
        ASSERT_THAT(b->IsAuthorized(), Eq(false));
        ASSERT_THAT(b->GetId(), Eq(_id));
        ASSERT_THAT(b->GetApiversion(), Eq("0.0.0"));
    });
}

TEST_F(TestDefaultAuthenticator, AuthenticateSuccessCorrectedDeviceType) {
    _appName = "thisisatoolong#applicationname";
    _deviceName = "this#devicename#isalsotoolong";
    _appSettings->SetAppName(_appName);
    _appSettings->SetDeviceName(_deviceName);
    const std::string body =
        "{\"devicetype\": \"thisisatoolongapplic#thisdevicenameisals\", \"generateclientkey\": true\n}";

    EXPECT_CALL(*_mockHttpPtr, Execute(MatchHttpRequest(HTTP_REQUEST_POST, getAuthUrl(),
        body))).Times(1).WillOnce(Invoke(SetHttpAuthenticateResponseSuccess));

    _defaultAuthenticator->Authenticate(_bridge, _appSettings, [this](BridgePtr b) {
        checkBasicBridgeAttributes(b);
        ASSERT_THAT(b->GetClientKey(), Eq(s_clientkey));
        ASSERT_THAT(b->GetApiversion(), Eq(_apiVersion));
    });
}

}

