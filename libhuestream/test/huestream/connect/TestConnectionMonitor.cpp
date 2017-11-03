#include <huestream/connect/FeedbackMessage.h>
#include <huestream/connect/ConnectionMonitor.h>
#include <test/huestream/_mock/MockBridgeStateChecker.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::_;
using ::testing::Sequence;
using ::testing::Property;
using ::testing::AllOf;
using ::testing::Pointee;

namespace huestream {
    class TestConnectionMonitor : public testing::Test {
    public:
        virtual void SetUp() {
            _bridgeSettings = std::make_shared<BridgeSettings>();
            _bridge1 = std::make_shared<Bridge>(_bridgeSettings);
            _bridge1->SetId("bridge1");
            _bridge2 = std::make_shared<Bridge>(_bridgeSettings);
            _bridge2->SetId("bridge2");
            _mockBridgeStateChecker = std::shared_ptr<MockBridgeStateChecker>(new MockBridgeStateChecker());
            _connectionMonitor = std::make_shared<ConnectionMonitor>(std::static_pointer_cast<IBridgeStreamingChecker>(_mockBridgeStateChecker));
        }

        virtual void TearDown() {
            _connectionMonitor->Stop();
        }

        BridgePtr _bridge1;
        BridgePtr _bridge2;
        BridgeSettingsPtr _bridgeSettings;
        std::shared_ptr<MockBridgeStateChecker> _mockBridgeStateChecker;
        std::shared_ptr<ConnectionMonitor> _connectionMonitor;

    };

TEST_F(TestConnectionMonitor, Start_KicksCheckerPeriodically) {
    EXPECT_CALL(*_mockBridgeStateChecker, Check(_bridge1)).Times(2);
    _connectionMonitor->Start(_bridge1, 500);
    std::this_thread::sleep_for(std::chrono::milliseconds(1250));
}

TEST_F(TestConnectionMonitor, Start_OverridesPreviousSettings) {
    EXPECT_CALL(*_mockBridgeStateChecker, Check(_bridge1)).Times(2);
    _connectionMonitor->Start(_bridge1, 500);
    std::this_thread::sleep_for(std::chrono::milliseconds(1250));

    EXPECT_CALL(*_mockBridgeStateChecker, Check(_bridge2)).Times(4);
    _connectionMonitor->Start(_bridge2, 250);
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(250 * 4 + 250 * 0.5)));
}


}  // namespace huestream