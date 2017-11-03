/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_CONNECTIONFLOW_H_
#define HUESTREAM_CONNECT_CONNECTIONFLOW_H_

#include <huestream/common/data/Bridge.h>
#include <huestream/stream/IStream.h>
#include <huestream/connect/FeedbackMessage.h>
#include <huestream/connect/IConnectionFlowFactory.h>
#include <huestream/connect/IConnectionFlow.h>
#include <huestream/connect/IBridgeSearcher.h>

#include <memory>
#include <string>
#include <functional>
#include <map>

namespace huestream {

class ConnectionFlow: public IConnectionFlow {
 public:
    ConnectionFlow(ConnectionFlowFactoryPtr factory, StreamPtr stream, BridgeSettingsPtr bridgeSettings, AppSettingsPtr appSettings, BridgeStorageAccessorPtr storageAccessor);

    ~ConnectionFlow() override;

    void Load() override;

    void ConnectToBridge() override;

    void ConnectToBridgeBackground() override;

    void ConnectToBridgeWithIp(const std::string &ipAddress) override;

    void ConnectToNewBridge() override;

    void SetManual(BridgePtr bridge) override;

    void ResetBridge() override;

    void ResetAll() override;

    void SelectGroup(std::string id) override;

    void StartStream(StreamPtr stream) override;

    void StopStream(StreamPtr stream) override;

    void Abort() override;

    void OnBridgeMonitorEvent(const FeedbackMessage &message) override;

    void NewMessage(const FeedbackMessage &message) override;

    void SetFeedbackMessageCallback(FeedbackMessageCallback callback) override;
    ConnectionFlowState GetState() override;

 protected:
    void DoLoad();

    void DoConnectToBridge();

    void DoConnectToBridgeBackground();

    void DoConnectToBridgeWithIp(const std::string &ipAddress);

    void DoConnectToNewBridge();

    void DoSetManual(BridgePtr bridge);

    void DoReset(bool onlyActiveBridge);

    void DoSelectGroup(std::string id);

    void DoStartStream();

    void DoStopStream();

    void DoAbort();

    void DoOnBridgeMonitorEvent(const FeedbackMessage &message);

    bool Start(FeedbackMessage::RequestType request);

    void StartLoading();

    void LoadingCompleted(OperationResult result, HueStreamDataPtr persistentData);

    void StartBridgeSearch();

    void BridgeSearchCompleted(BridgeListPtr bridges);

    BridgeListPtr FilterInvalidAndDuplicateIpAddresses(BridgeListPtr bridges);

    bool ContainsValidBridge(BridgeListPtr bridges);

    void StartAuthentication(BridgePtr bridge);

    void StartAuthentication(BridgeListPtr bridges);

    void AuthenticationCompleted(BridgePtr bridge);

    void PushLinkBridge(BridgePtr bridge);

    void StartRetrieveFullConfig();

    void RetrieveFullConfigCompleted(OperationResult result, BridgePtr bridge);

    void ReportActionRequired();

    void ActivateStreaming();

    void DeactivateStreaming();

    void SetNewActiveBridge(BridgePtr bridge);

    void ClearBridge();

    void StartSaving();

    void SavingCompleted(OperationResult r);

    void Finish();

    ConnectionFlowFactoryPtr _factory;
    AppSettingsPtr _appSettings;
    BridgeSettingsPtr _bridgeSettings;
    BridgeStorageAccessorPtr _storageAccessor;
    BridgeSearcherPtr _bridgeSearcher;
    FullConfigRetrieverPtr _fullconfigRetriever;
    ConnectionFlowState _state;
    FeedbackMessage::RequestType _request;
    size_t _ongoingAuthenticationCount;
    BridgeListPtr _backgroundDiscoveredBridges;
    StreamPtr _stream;
    FeedbackMessageCallback _feedbackMessageCallback;
    HueStreamDataPtr _persistentData;

};
}  // namespace huestream

#endif  // HUESTREAM_CONNECT_CONNECTIONFLOW_H_
