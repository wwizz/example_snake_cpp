#include <huestream/connect/FeedbackMessage.h>
#include "huestream/common/language/DummyTranslator.h"
#include <huestream/config/Config.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
namespace huestream {

class TestFeedbackMessage : public testing::Test {
 public:
    virtual void SetUp() {
        _bridgeSettings = std::make_shared<BridgeSettings>();
        _bridge = std::make_shared<Bridge>(_bridgeSettings);
        FeedbackMessage::SetTranslator(std::make_shared<DummyTranslator>("en"));

    }

    virtual void TearDown() {
        FeedbackMessage::SetTranslator(MessageTranslatorPtr());
    }

    BridgePtr _bridge;
    BridgeSettingsPtr _bridgeSettings;
};

TEST_F(TestFeedbackMessage, Message) {
    FeedbackMessage feedbackMessageInvBridgeFound(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_FINISH_SEARCHING_INVALID_BRIDGES_FOUND, _bridge);
    ASSERT_EQ(FeedbackMessage::ID_FINISH_SEARCHING_INVALID_BRIDGES_FOUND,     feedbackMessageInvBridgeFound.GetId());
    ASSERT_EQ("ID_FINISH_SEARCHING_INVALID_BRIDGES_FOUND",                    feedbackMessageInvBridgeFound.GetTag());
    ASSERT_EQ(FeedbackMessage::FEEDBACK_TYPE_INFO,                            feedbackMessageInvBridgeFound.GetMessageType());
    ASSERT_EQ("REQUEST_TYPE_INTERNAL: ID_FINISH_SEARCHING_INVALID_BRIDGES_FOUND", feedbackMessageInvBridgeFound.GetDebugMessage());
    ASSERT_EQ("Internal error",                                               feedbackMessageInvBridgeFound.GetUserMessage());

    auto bridgeList = std::make_shared<BridgeList>();
    bridgeList->push_back(_bridge);
    FeedbackMessage feedbackMessageValid(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_PRESS_PUSH_LINK, _bridge, bridgeList);
    ASSERT_EQ(FeedbackMessage::ID_PRESS_PUSH_LINK,                                 feedbackMessageValid.GetId());
    ASSERT_EQ("ID_PRESS_PUSH_LINK",                               feedbackMessageValid.GetTag());
    ASSERT_EQ(FeedbackMessage::FEEDBACK_TYPE_USER,                feedbackMessageValid.GetMessageType());
    ASSERT_EQ("REQUEST_TYPE_INTERNAL: ID_PRESS_PUSH_LINK",            feedbackMessageValid.GetDebugMessage());
    ASSERT_EQ("1 Hue Bridge found. Press the push-link button on the Hue Bridge.",    feedbackMessageValid.GetUserMessage());
}
}  // namespace huestream