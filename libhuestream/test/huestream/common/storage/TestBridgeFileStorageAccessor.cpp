#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <huestream/common/data/BridgeSettings.h>
#include <huestream/connect/BridgeFileStorageAccessor.h>

#include <fstream>
#include <memory>

using namespace huestream;
using namespace std;

class TestBridgeFileStorageAccessor : public testing::Test {
public:
    BridgeStorageAccessorPtr _bridgeStorageAccessorPtr;
    const char *_fileName;

    virtual void SetUp() {
        _fileName = "TestBridgeFileStorageAccessor.bridge.json";
        FILE *fp = fopen(_fileName, "w");
        const char *data = "{\"type\":\"huestream.Bridge\","
                "\"Name\":\"\","
                "\"ModelId\":\"BSB002\","
                "\"SoftwareVersion\":\"01035797\","
                "\"Apiversion\":\"1.17.0\","
                "\"Id\":\"00:11:22:33:44\","
                "\"IpAddress\":\"192.168.1.1\","
                "\"IsValidIp\":true,"
                "\"IsAuthorized\":true,"
                "\"ClientKey\":\"DD129216F1A50E5D1C0CB356325745F2\","
                "\"User\":\"HJD77jsjs-7883kkKS@\","
                "\"SelectedGroup\":123}";
        if (fp != NULL)
        {
            fputs(data, fp);
            fclose(fp);
        }
    }

    virtual void TearDown() {
        _bridgeStorageAccessorPtr.reset();
        std::remove(_fileName);
    }
};

TEST_F(TestBridgeFileStorageAccessor, Load) {
    _bridgeStorageAccessorPtr = std::make_shared<BridgeFileStorageAccessor>(_fileName, std::make_shared<BridgeSettings>());
    _bridgeStorageAccessorPtr->Load([this](OperationResult oRes, HueStreamDataPtr bPtr){
        ASSERT_EQ(OperationResult::OPERATION_SUCCESS, oRes);
        ASSERT_NE(nullptr, bPtr);
    });
}

TEST_F(TestBridgeFileStorageAccessor, Save) {
    auto bridge = std::make_shared<Bridge>(std::make_shared<BridgeSettings>());
    bridge->SetId("00:11:22:33:44");
    bridge->SetUser("HJD77jsjs-7883kkKS@");
    bridge->SetModelId("BSB002");
    bridge->SetApiversion("1.17.0");
    bridge->SetClientKey("DD129216F1A50E5D1C0CB356325745F2");
    {
        auto group1 = std::make_shared<Group>();
        group1->SetId("123");
        group1->SetName("My entertainment setup1");
        group1->AddLight("22", 0.1, 0.1);
        group1->AddLight("12", 0.3, 0.2);
    }
    bridge->SetSelectedGroup("123");
    bridge->SetIsValidIp(true);
    bridge->SetIsAuthorized(true);
    bridge->SetIpAddress("192.168.1.1");
    auto hueStreamData = std::make_shared<HueStreamData>(std::make_shared<BridgeSettings>());
    hueStreamData->SetActiveBridge(bridge);

    _bridgeStorageAccessorPtr = std::make_shared<BridgeFileStorageAccessor>("bridge.json", std::make_shared<BridgeSettings>());
    _bridgeStorageAccessorPtr->Save(hueStreamData, [this](OperationResult oRes){
        ASSERT_EQ(OperationResult::OPERATION_SUCCESS, oRes);
    });
    ASSERT_TRUE(std::ifstream("bridge.json"));
}
