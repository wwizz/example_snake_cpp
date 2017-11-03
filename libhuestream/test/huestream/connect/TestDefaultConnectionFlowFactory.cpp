#include <huestream/connect/ConnectionFlowFactory.h>
#include <huestream/common/http/HttpClient.h>
#include <huestream/common/data/BridgeSettings.h>
#include <huestream/connect/MessageDispatcher.h>
#include <huestream/connect/BridgeFileStorageAccessor.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>


namespace huestream {

class TestDefaultConnectionFlowFactory : public testing::Test {
 protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(TestDefaultConnectionFlowFactory, GetFactory) {
    auto settings = std::make_shared<BridgeSettings>();
    auto accessor = std::make_shared<BridgeFileStorageAccessor>("bridge.json", settings);
    auto factory = std::make_shared<ConnectionFlowFactory>(settings,
                                                                  std::make_shared<HttpClient>(),
                                                                  std::make_shared<MessageDispatcher>(),
                                                                  std::static_pointer_cast<IBridgeStorageAccessor>(accessor));

    ASSERT_NE(nullptr, factory->CreateAuthenticator());
    ASSERT_NE(nullptr, factory->CreateFullConfigRetriever());
    ASSERT_NE(nullptr, factory->CreateSearcher());
    ASSERT_NE(nullptr, factory->GetMessageDispatcher());
    ASSERT_NE(nullptr, factory->GetStorageAccesser());
}

}  // namespace huestream