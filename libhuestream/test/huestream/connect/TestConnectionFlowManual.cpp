#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <huestream/connect/ConnectionFlow.h>
#include "test/huestream/_mock/MockBridgeAuthenticator.h"
#include "test/huestream/_mock/MockBridgeSearcher.h"
#include "TestableConnectionFlow.h"
#include "test/huestream/_mock/MockConnectionFlowFactory.h"
#include "test/huestream/_mock/MockBridgeStorageAccessor.h"
#include "test/huestream/_stub/StubMessageDispatcher.h"
#include "TestConnectionFlowBase.h"

using namespace testing;
using namespace huestream;


class TestConnectionFlow_Manual : public TestConnectionFlowBase {
public:

    void connect_manual_ip_load_retrieve_push_link(int index) {
        expect_message(FeedbackMessage::ID_USERPROCEDURE_STARTED, FeedbackMessage::FEEDBACK_TYPE_INFO);

        expect_storage_accessor_load_return_data();
        expect_small_config_retrieval_return_data(index);
        expect_message(FeedbackMessage::ID_START_AUTHORIZING, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_message(FeedbackMessage::ID_PRESS_PUSH_LINK, FeedbackMessage::FEEDBACK_TYPE_USER);
        expect_on_authenticator_authenticate(index);

        std::string ipAddress = _bridges->at(index)->GetIpAddress();
        _connectionFlow->ConnectToBridgeWithIp(ipAddress);
        _messageDispatcher->ExecutePendingActions();
    }

    void set_manual(int index) {
        _bridges->at(index)->SetUser("HSJKHSDKJHKJDHIUHHFYU&e213");
        _bridges->at(index)->SetClientKey("00000000000000000000000000000000");

        _connectionFlow->SetManual(_bridges->at(index));
    }

    void set_manual_load_retrieve_config(int index, int numGroups, int numBridges = 1, bool wasPreviousBridgeStreaming = false, int previousBridgeIndex = 0) {
        expect_message(FeedbackMessage::ID_USERPROCEDURE_STARTED, FeedbackMessage::FEEDBACK_TYPE_INFO);

        if (wasPreviousBridgeStreaming) {
            EXPECT_CALL(*_stream, Stop(_bridges->at(previousBridgeIndex))).WillOnce(Invoke(DeactivateBridge));
        }

        expect_storage_accessor_load_return_data();
        expect_small_config_retrieval_return_data(index);
        expect_initiate_full_config_retrieval();
        set_manual(index);

        _messageDispatcher->ExecutePendingActions();

        retrieve_full_config(index, numGroups, numBridges);
        _persistentData->SetActiveBridge(_bridges->at(index));
    }

    void reset_bridge_returns_empty_bridge(bool resetAllBridges = false, int numberOfBridges = 1) {
        expect_message(FeedbackMessage::ID_USERPROCEDURE_STARTED, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_message(FeedbackMessage::ID_DONE_RESET, FeedbackMessage::FEEDBACK_TYPE_INFO, std::make_shared<Bridge>(std::make_shared<BridgeSettings>()), BRIDGE_EMPTY);
        expect_on_storage_accesser_save(numberOfBridges, "");
        if (resetAllBridges) {
            _connectionFlow->ResetAll();
        } else {
            _connectionFlow->ResetBridge();
        }
        _messageDispatcher->ExecutePendingActions();
    }

};

INSTANTIATE_TEST_CASE_P(connect_with_manual_ip,
                        TestConnectionFlow_Manual, Values(0, 1));

TEST_P(TestConnectionFlow_Manual, connect_with_manual_ip) {
    _settings->SetAutoStartAtConnection(int2bool(GetParam()));
    connect_manual_ip_load_retrieve_push_link(0);
    finish_authorization_successfully_and_retrieve_full_config(0);
    finish(_bridges->at(0));
}

INSTANTIATE_TEST_CASE_P(connect_with_manual_ip_and_credentials,
                        TestConnectionFlow_Manual, Values(0, 1));

TEST_P(TestConnectionFlow_Manual, connect_with_manual_ip_and_credentials) {
    _settings->SetAutoStartAtConnection(int2bool(GetParam()));
    set_manual_load_retrieve_config(1, 1);
    finish(_bridges->at(1));
}

INSTANTIATE_TEST_CASE_P(reset_bridge_which_was_connected_vs_not_connected,
    TestConnectionFlow_Manual, Values(0, 1));

TEST_P(TestConnectionFlow_Manual, reset_bridge_which_was_connected_vs_not_connected) {
    _settings->SetAutoStartAtConnection(false);
    set_manual_load_retrieve_config(1, 1);
    finish(_bridges->at(1));

    auto wasBridgeConnected = int2bool(GetParam());
    if (!wasBridgeConnected) {
        _bridges->at(1)->SetIsValidIp(false);
        FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_BRIDGE_DISCONNECTED, _bridges->at(1));
    }
    reset_bridge_returns_empty_bridge();
    if (wasBridgeConnected) {
        expect_message(FeedbackMessage::ID_BRIDGE_DISCONNECTED, FeedbackMessage::FEEDBACK_TYPE_INFO);
    }
    finish_without_stream_start(false, false);
}

INSTANTIATE_TEST_CASE_P(reset_single_bridge_vs_reset_all_bridges,
    TestConnectionFlow_Manual, Values(0, 1));

TEST_P(TestConnectionFlow_Manual, reset_single_bridge_vs_reset_all_bridges) {
    _settings->SetAutoStartAtConnection(false);
    set_manual_load_retrieve_config(1, 1);
    finish(_bridges->at(1));

    set_manual_load_retrieve_config(2, 1, 2, false, 1);
    finish_without_stream_start(true, false, false);

    auto resetAllBridges = int2bool(GetParam());
    if (resetAllBridges) {
        reset_bridge_returns_empty_bridge(resetAllBridges);
    } else {
        reset_bridge_returns_empty_bridge(resetAllBridges, 2);
    }

    expect_message(FeedbackMessage::ID_BRIDGE_DISCONNECTED, FeedbackMessage::FEEDBACK_TYPE_INFO);
    finish_without_stream_start(false, false);
}

TEST_F(TestConnectionFlow_Manual, connect_with_manual_ip_followed_by_connect_with_manual_ip_and_credentials) {
    _settings->SetAutoStartAtConnection(false);
    connect_manual_ip_load_retrieve_push_link(0);

    finish_authorization_successfully_and_retrieve_full_config(0);
    finish(_bridges->at(0));

    set_manual_load_retrieve_config(1, 2, 2, false, 0);
    finish_without_stream_start(false, false, true);
}

TEST_F(TestConnectionFlow_Manual, connect_with_manual_ip_abort_and_retry) {
    connect_manual_ip_load_retrieve_push_link(0);

    expect_no_actions();
    _connectionFlow->ConnectToBridgeWithIp(_bridges->at(1)->GetIpAddress());
    _messageDispatcher->ExecutePendingActions();

    abort_finalizes();

    connect_manual_ip_load_retrieve_push_link(2);
    finish_authorization_successfully_and_retrieve_full_config(2);
    finish(_bridges->at(2));
}

TEST_F(TestConnectionFlow_Manual, connect_with_manual_ip_and_credentials_abort_and_retry) {
    expect_message(FeedbackMessage::ID_USERPROCEDURE_STARTED, FeedbackMessage::FEEDBACK_TYPE_INFO);
    expect_storage_accessor_load_return_data();
    expect_small_config_retrieval_return_data(0);
    expect_initiate_full_config_retrieval();
    set_manual(0);
    _messageDispatcher->ExecutePendingActions();

    expect_no_actions();
    set_manual(1);
    _messageDispatcher->ExecutePendingActions();

    abort_finalizes();

    set_manual_load_retrieve_config(2, 1, 2, false, 1);
    finish(_bridges->at(2));
}

TEST_P(TestConnectionFlow_Manual, add_manual_already_known_bridge_doesnt_duplicate_entry) {
    _settings->SetAutoStartAtConnection(false);

    set_manual_load_retrieve_config(0, 1);
    finish_without_stream_start(true);

    set_manual_load_retrieve_config(1, 1, 2, false, 0);
    finish_without_stream_start(true, false, false);

    set_manual_load_retrieve_config(0, 1, 2, false, 1);
    finish_without_stream_start(true, false, false);

    set_manual_load_retrieve_config(2, 1, 3, false, 0);
    finish_without_stream_start(true, false, false);

    set_manual_load_retrieve_config(1, 1, 3, false, 2);
    finish_without_stream_start(true, false, false);

    set_manual_load_retrieve_config(0, 1, 3, false, 1);
    finish_without_stream_start(true, false, false);
}

TEST_P(TestConnectionFlow_Manual, add_manual_when_already_streaming_first_stops_streaming) {
    _settings->SetAutoStartAtConnection(true);

    set_manual_load_retrieve_config(0, 1);
    finish(_bridges->at(0));
    ASSERT_EQ(_bridges->at(0)->GetStatus(), BRIDGE_STREAMING);

    set_manual_load_retrieve_config(1, 1, 2, true, 0);
    finish_with_stream_start(_bridges->at(1), false, false, false);
}

TEST_F(TestConnectionFlow_Manual, api_version_empty__expect_small_and_full_config_retrieval) {
    set_manual_load_retrieve_config(3, 1);
}
