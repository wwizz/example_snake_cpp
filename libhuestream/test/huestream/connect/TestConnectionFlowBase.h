#ifndef LIBHUESTREAM_TESTCONNECTIONFLOWBASE_H
#define LIBHUESTREAM_TESTCONNECTIONFLOWBASE_H


#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <huestream/connect/ConnectionFlow.h>
#include <huestream/config/Config.h>
#include "test/huestream/_mock/MockBridgeAuthenticator.h"
#include "test/huestream/_mock/MockBridgeSearcher.h"
#include "TestableConnectionFlow.h"
#include "test/huestream/_mock/MockConnectionFlowFactory.h"
#include "test/huestream/_mock/MockBridgeStorageAccesser.h"
#include "test/huestream/_stub/StubMessageDispatcher.h"
#include "test/huestream/_mock/MockFullConfigRetriever.h"
#include "test/huestream/_mock/MockStream.h"

using namespace testing;
using namespace huestream;

class TestConnectionFlowBase : public testing::TestWithParam<int> {
public:
    shared_ptr<Config> _config;
    shared_ptr<AppSettings> _settings;
    shared_ptr<BridgeList> _bridges;
    std::shared_ptr<TestableConnectionFlow> _connectionFlow;
    shared_ptr<MockConnectionFlowFactory> _factory;
    shared_ptr<vector<shared_ptr<MockBridgeSearcher>>> _searchers;
    shared_ptr<vector<shared_ptr<MockBridgeAuthenticator>>> _authenticators;
    shared_ptr<StubMessageDispatcher> _messageDispatcher;
    shared_ptr<MockBridgeStorageAccesser> _storageAccesser;
    shared_ptr<MockStream> _stream;
    Sequence s;

    void SetUp() override;

    void TearDown() override;

    bool int2bool(int param);

    void expect_message(FeedbackMessage::Id id, FeedbackMessage::FeedbackType type);

    void expect_message(FeedbackMessage::Id id, FeedbackMessage::FeedbackType type, BridgePtr bridge);

    void expect_message(FeedbackMessage::Id id, FeedbackMessage::FeedbackType type, BridgePtr bridge, BridgeStatus status);

    void expect_no_actions();

    void expect_on_searcher_search_new(bool bruteForce);

    void expect_on_searcher_abort();

    void expect_on_authenticator_authenticate(int index);

    void expect_on_storage_accesser_load();

    void connect_starts_bridge_loading(bool background = false);

    void finish_search_with_no_bridges(shared_ptr<MockBridgeSearcher> searcher);

    void finish_search_with_one_bridge(shared_ptr<MockBridgeSearcher> searcher, int index);

    void finish_search_with_three_bridges(shared_ptr<MockBridgeSearcher> searcher);

    void finish_search_with_only_v1_bridge(shared_ptr<MockBridgeSearcher> searcher);

    void searching_with_three_bridges_found_starts_pushlink_on_all_three_bridges();

    void searching_with_one_bridge_found_starts_pushlink();

    void searching_background_with_one_bridge_found_finishes(int index = 0);

    void searching_background_with_no_bridge_found_finishes();

    void searching_background_with_only_v1_bridge_found_finishes();

    void searching_once_with_no_bridges_found_starts_new_bridge_search();

    void searching_twice_with_no_bridges_found_finalizes();

    void searching_twice_with_no_bridges_found_finalizes(BridgePtr bridge, bool existingBridge);

    void authorize_bridge(int index);

    void finish_authorization_successfully_and_retrieve_full_config(int index, unsigned int numGroups = 1, unsigned int numBridges = 1);

    void finish_authorization_successfully(int index);

    void expect_initiate_full_config_retrieval();

    void retrieve_full_config(int index, unsigned int numGroups, unsigned int numBridges = 1);

    void finish(BridgePtr bridge);

    void finish_without_stream_start(bool completed = true);

    void finish_with_stream_start(BridgePtr bridge);

    void abort_finalizes();

    void searching_with_abort_finalizes();

    void select_group_existing(int bridgeindex, std::string groupid);

    void select_group_not_existing(int bridgeindex, std::string groupid);

    shared_ptr<MockBridgeSearcher> get_last_searcher();

    shared_ptr<MockBridgeAuthenticator> get_last_authenticator();

    shared_ptr<MockFullConfigRetriever> _fullConfigRetriever;

    void expect_on_storage_accesser_save();

    void expect_on_storage_accesser_save(BridgePtr bridge);

    void expect_on_storage_accesser_save(int numberOfBridges, std::string activeBridgeId);

    void expect_on_full_config_retriever_execute() const {
        
        EXPECT_CALL(*_fullConfigRetriever, Execute(_, _))
                .Times(1)
                .WillOnce(DoAll(
                        SaveArg<0>(&_fullConfigRetriever->Bridge),
                        SaveArg<1>(&_fullConfigRetriever->RetrieveCallback),
                        Return(true)));
    }

    void set_bridge_in_full_config(unsigned int numGroups) const {
        if (_fullConfigRetriever->Bridge == nullptr) {
            return;
        }
        ASSERT_TRUE(numGroups <= 2);

        _fullConfigRetriever->Bridge->SetIsValidIp(true);
        _fullConfigRetriever->Bridge->SetModelId("BSB002");
        _fullConfigRetriever->Bridge->SetApiversion("1.22.0");
        auto groups = std::make_shared<GroupList>();

        if (numGroups >= 1) {
            auto group1 = std::make_shared<Group>();
            group1->SetId("12");
            group1->SetName("My Entertainment Group1");
            group1->AddLight("1", 0.5, 0.4);
            group1->AddLight("2", 0.4, 0.2);
            groups->push_back(group1);
        }
        if (numGroups >= 2) {
            auto group2 = std::make_shared<Group>();
            group2->SetId("13");
            group2->SetName("My Entertainment Group2");
            group2->AddLight("3", 0.5, 0.4);
            group2->AddLight("4", 0.4, 0.2);
            groups->push_back(group2);
        }
        _fullConfigRetriever->Bridge->SetGroups(groups);
    }

    void set_bridge_not_authorized_for_full_config() const {
        if (_fullConfigRetriever->Bridge == nullptr) {
            return;
        }
        _fullConfigRetriever->Bridge->SetIsAuthorized(false);
    }

    void set_bridge_not_found_for_full_config() const {
        if (_fullConfigRetriever->Bridge == nullptr) {
            return;
        }
        _fullConfigRetriever->Bridge->SetIsValidIp(false);
    }

    static bool ActivateBridge(BridgePtr bridge) {
        if (bridge->IsValidGroupSelected()) {
            bridge->GetGroup()->SetActive(true);
            bridge->GetGroup()->SetOwner(bridge->GetUser());
        }
        return true;
    }

    static bool FailActivateBridgeBusy(BridgePtr bridge) {
        bridge->SetIsBusy(true);
        return false;
    }

    static void DeactivateBridge(BridgePtr bridge) {
        bridge->GetGroup()->SetActive(false);
    }

    static void PrintDebugMsg(const FeedbackMessage &m) {
        //change below comment for easier debugging on connectionflow tests
        (void)m;
        //std::cout << m.GetDebugMessage() << std::endl;
    }
};

#endif  // LIBHUESTREAM_TESTCONNECTIONFLOWBASE_H
