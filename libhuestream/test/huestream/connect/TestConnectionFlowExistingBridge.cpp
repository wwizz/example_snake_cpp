#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <huestream/connect/ConnectionFlow.h>
#include "test/huestream/_mock/MockBridgeAuthenticator.h"
#include "test/huestream/_mock/MockBridgeSearcher.h"
#include "TestableConnectionFlow.h"
#include "test/huestream/_mock/MockConnectionFlowFactory.h"
#include "test/huestream/_stub/StubMessageDispatcher.h"
#include "TestConnectionFlowBase.h"
#include "huestream/config/Config.h"

using namespace huestream;


class TestConnectionFlow_ExistingBridge : public TestConnectionFlowBase {
public:
    HueStreamDataPtr _existingBridgeData;

    void SetUp() override {
        TestConnectionFlowBase::SetUp();

        auto existingBridge = std::make_shared<Bridge>("001788FFFE2007DD", "192.168.1.37", true, std::make_shared<BridgeSettings>());
        existingBridge->SetModelId("BSB002");
        existingBridge->SetApiversion("1.22.0");
        existingBridge->SetUser("HSJKHSDKJHKJDHIUHHFYU&e213");
        existingBridge->SetClientKey("00000000000000000000000000000000");
        existingBridge->SetIsAuthorized(true);
        
        _existingBridgeData = std::make_shared<HueStreamData>(std::make_shared<BridgeSettings>());
        _existingBridgeData->SetActiveBridge(existingBridge);
    }

    void addAnotherKnownBridge(std::string id) {
        auto existingBridge = std::make_shared<Bridge>(id, "192.168.1.44", true, std::make_shared<BridgeSettings>());
        existingBridge->SetModelId("BSB002");
        existingBridge->SetApiversion("1.23.0");
        existingBridge->SetUser("fdsajfklasdjfkldsa");
        existingBridge->SetClientKey("00000000000001337000000000000000");
        existingBridge->SetIsAuthorized(true);

        _existingBridgeData->SetActiveBridge(existingBridge);
    }

    void finish_loading() {
        expect_message(FeedbackMessage::ID_FINISH_LOADING_BRIDGE_CONFIGURED, FeedbackMessage::FEEDBACK_TYPE_INFO, _existingBridgeData->GetActiveBridge());
        expect_initiate_full_config_retrieval();

        _storageAccesser->load_callback(OPERATION_SUCCESS, _existingBridgeData);
        _messageDispatcher->ExecutePendingActions();
    }

    void finish_loading_execute_fullconfig_with_valid_bridge() {
        finish_loading();

        set_bridge_in_full_config(1);
        expect_on_storage_accesser_save();
        expect_message(FeedbackMessage::ID_FINISH_RETRIEVING_READY_TO_START, FeedbackMessage::FEEDBACK_TYPE_INFO, _existingBridgeData->GetActiveBridge(), BRIDGE_READY);

        _fullConfigRetriever->RetrieveCallback(OPERATION_SUCCESS, _fullConfigRetriever->Bridge);
        _messageDispatcher->ExecutePendingActions();
    }

    void finish_loading_execute_fullconfig_with_no_groups() {
        finish_loading();

        set_bridge_in_full_config(0);
        expect_on_storage_accesser_save();
        expect_message(FeedbackMessage::ID_FINISH_RETRIEVING_ACTION_REQUIRED, FeedbackMessage::FEEDBACK_TYPE_INFO, _existingBridgeData->GetActiveBridge(), BRIDGE_NO_GROUP_AVAILABLE);
        expect_message(FeedbackMessage::ID_DONE_ACTION_REQUIRED, FeedbackMessage::FEEDBACK_TYPE_INFO, _existingBridgeData->GetActiveBridge(), BRIDGE_NO_GROUP_AVAILABLE);
        expect_message(FeedbackMessage::ID_NO_GROUP_AVAILABLE, FeedbackMessage::FEEDBACK_TYPE_USER, _existingBridgeData->GetActiveBridge());

        _fullConfigRetriever->RetrieveCallback(OPERATION_SUCCESS, _fullConfigRetriever->Bridge);
        _messageDispatcher->ExecutePendingActions();
    }

    void finish_loading_execute_fullconfig_not_authorized_starts_pushlink(int index) {
        finish_loading();

        set_bridge_not_authorized_for_full_config();
        expect_message(FeedbackMessage::ID_FINISH_RETRIEVING_FAILED, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_message(FeedbackMessage::ID_START_AUTHORIZING, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_message(FeedbackMessage::ID_PRESS_PUSH_LINK, FeedbackMessage::FEEDBACK_TYPE_USER);

        expect_on_authenticator_authenticate(index);

        _fullConfigRetriever->RetrieveCallback(OPERATION_FAILED, _fullConfigRetriever->Bridge);
        _messageDispatcher->ExecutePendingActions();
    }

    void finish_loading_execute_fullconfig_not_found_starts_bridgesearch() {
        finish_loading();

        set_bridge_not_found_for_full_config();
        expect_message(FeedbackMessage::ID_FINISH_RETRIEVING_FAILED, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_message(FeedbackMessage::ID_START_SEARCHING, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_on_searcher_search_new(false);

        _fullConfigRetriever->RetrieveCallback(OPERATION_FAILED, _fullConfigRetriever->Bridge);
        _messageDispatcher->ExecutePendingActions();
    }

    void finish_loading_start_bridgesearch() {
        expect_message(FeedbackMessage::ID_FINISH_LOADING_BRIDGE_CONFIGURED, FeedbackMessage::FEEDBACK_TYPE_INFO, _existingBridgeData->GetActiveBridge());
        expect_message(FeedbackMessage::ID_START_SEARCHING, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_on_searcher_search_new(false);

        _storageAccesser->load_callback(OPERATION_SUCCESS, _existingBridgeData);
        _messageDispatcher->ExecutePendingActions();
    }

    void connect_bridge_found_on_different_ip(int index) {
        connect_starts_bridge_loading();
        finish_loading_execute_fullconfig_not_found_starts_bridgesearch();

        expect_message(FeedbackMessage::ID_FINISH_SEARCH_BRIDGES_FOUND, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_initiate_full_config_retrieval();
        finish_search_with_three_bridges(get_last_searcher());

        retrieve_full_config(index, 1);
    }

    void connect_bridge_lost_other_known_bridge_found(int index, int numBridges = 2) {
        connect_starts_bridge_loading();
        finish_loading_execute_fullconfig_not_found_starts_bridgesearch();

        expect_message(FeedbackMessage::ID_FINISH_SEARCH_BRIDGES_FOUND, FeedbackMessage::FEEDBACK_TYPE_INFO);
        expect_initiate_full_config_retrieval();
        finish_search_with_three_bridges(get_last_searcher());

        retrieve_full_config(index, 1, numBridges);
    }

    void connect_bridge_is_lost() {
        connect_starts_bridge_loading();
        finish_loading_execute_fullconfig_not_found_starts_bridgesearch();

        searching_once_with_no_bridges_found_starts_new_bridge_search();
        searching_twice_with_no_bridges_found_finalizes(_existingBridgeData->GetActiveBridge(), true);
        ASSERT_THAT(_existingBridgeData->GetActiveBridge()->IsAuthorized(), Eq(true));
        ASSERT_THAT(_existingBridgeData->GetActiveBridge()->IsValidIp(), Eq(false));
    }
};

INSTANTIATE_TEST_CASE_P(ValidBridgeInFullConfig,
                        TestConnectionFlow_ExistingBridge, Values(0, 1));

TEST_P(TestConnectionFlow_ExistingBridge, ValidBridgeInFullConfig) {
    _settings->SetAutoStartAtConnection(int2bool(GetParam()));
    connect_starts_bridge_loading();
    finish_loading_execute_fullconfig_with_valid_bridge();
    finish(_existingBridgeData->GetActiveBridge());
}

TEST_F(TestConnectionFlow_ExistingBridge, NoGroupsInFullConfig) {
    connect_starts_bridge_loading();
    finish_loading_execute_fullconfig_with_no_groups();
}

TEST_F(TestConnectionFlow_ExistingBridge, AuthorizationIsLost) {
    connect_starts_bridge_loading();
    _bridges->at(0) = _existingBridgeData->GetActiveBridge();
    finish_loading_execute_fullconfig_not_authorized_starts_pushlink(0);
    finish_authorization_successfully_and_retrieve_full_config(0);
}

TEST_F(TestConnectionFlow_ExistingBridge, IpAddressIsChanged) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());

    connect_bridge_found_on_different_ip(1);
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeIsLost) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());

    connect_bridge_is_lost();
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeWasPreviouslyLostAndNowBack) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());
    _existingBridgeData->GetActiveBridge()->SetIsValidIp(false);

    connect_starts_bridge_loading();
    finish_loading_execute_fullconfig_with_valid_bridge();
    finish(_existingBridgeData->GetActiveBridge());
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeWasPreviouslyLostAndNowOnDifferentIp) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());
    _existingBridgeData->GetActiveBridge()->SetIsValidIp(false);

    connect_bridge_found_on_different_ip(1);
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeWasPreviouslyLostAndNowStill) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());
    _existingBridgeData->GetActiveBridge()->SetIsValidIp(false);

    connect_bridge_is_lost();
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeIsV1) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());
    _existingBridgeData->GetActiveBridge()->SetModelId("BSB001");
    ASSERT_FALSE(_existingBridgeData->GetActiveBridge()->IsEmpty());
    ASSERT_FALSE(_existingBridgeData->GetActiveBridge()->IsConnectable());

    connect_starts_bridge_loading();
    finish_loading_start_bridgesearch();

    searching_with_three_bridges_found_starts_pushlink_on_all_three_bridges();
    finish_authorization_successfully_and_retrieve_full_config(2);
}

TEST_F(TestConnectionFlow_ExistingBridge, ConnectBackgroundValidBridgeInFullConfig) {
    connect_starts_bridge_loading(true);
    finish_loading_execute_fullconfig_with_valid_bridge();
    finish(_existingBridgeData->GetActiveBridge());
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeIsLostButAnotherKnownBridgeIsFound_TwoBridgesOneMatch) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());
    addAnotherKnownBridge("1234567890ABCDEF");

    connect_bridge_lost_other_known_bridge_found(1);
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeIsLostButAnotherKnownBridgeIsFound_ThreeBridgesOneMatch) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());
    addAnotherKnownBridge("1234567890ABCDEF");
    addAnotherKnownBridge("1234567890AAAAAA");

    connect_bridge_lost_other_known_bridge_found(1, 3);
}

TEST_F(TestConnectionFlow_ExistingBridge, BridgeIsLostButAnotherKnownBridgeIsFound_ThreeBridgesTwoMatchPicksMostRecent) {
    _existingBridgeData->GetActiveBridge()->SetId(_bridges->at(1)->GetId());
    addAnotherKnownBridge(_bridges->at(2)->GetId());
    addAnotherKnownBridge("1234567890AAAAAA");

    connect_bridge_lost_other_known_bridge_found(2, 3);
}

TEST_F(TestConnectionFlow_ExistingBridge, dont_start_if_disconnected) {
    _settings->SetAutoStartAtConnection(false);
    connect_starts_bridge_loading();
    finish_loading_execute_fullconfig_with_valid_bridge();
    finish(_existingBridgeData->GetActiveBridge());

    expect_message(FeedbackMessage::ID_USERPROCEDURE_STARTED, FeedbackMessage::FEEDBACK_TYPE_INFO);
    expect_initiate_full_config_retrieval();
    _connectionFlow->StartStream(nullptr);
    _messageDispatcher->ExecutePendingActions();

    set_bridge_not_found_for_full_config();
    expect_message(FeedbackMessage::ID_FINISH_RETRIEVING_FAILED, FeedbackMessage::FEEDBACK_TYPE_INFO);
    expect_message(FeedbackMessage::ID_START_SEARCHING, FeedbackMessage::FEEDBACK_TYPE_INFO);
    expect_on_searcher_search_new(false);

    _fullConfigRetriever->RetrieveCallback(OPERATION_FAILED, _fullConfigRetriever->Bridge);
    _messageDispatcher->ExecutePendingActions();
}

