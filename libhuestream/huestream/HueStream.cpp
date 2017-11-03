/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/


#include <huestream/HueStream.h>
#include <huestream/effect/lightscript/LightScript.h>
#include <huestream/common/http/HttpClient.h>
#include <huestream/connect/BridgeFileStorageAccessor.h>
#include <huestream/connect/BridgeStreamingChecker.h>
#include <huestream/common/time/TimeManager.h>
#include <huestream/common/time/TimeProviderProvider.h>
#include <huestream/stream/UdpConnector.h>
#include <huestream/stream/DtlsConnector.h>
#include <huestream/common/language/DummyTranslator.h>
#include <huestream/stream/DtlsEntropyProvider.h>
#include <huestream/config/ObjectBuilder.h>

#include <memory>
#include <string>
#include <utility>


namespace huestream {

PROP_IMPL(HueStream, BridgePtr, activeBridge, ActiveBridge);
PROP_IMPL(HueStream, StreamRenderCallback, streamRenderCallback, StreamRenderCallback);

HueStreamPtr HueStream::_hueStream;

HueStream::HueStream(ConfigPtr config) :
    HueStream(std::move(config),
              HttpClientFactory::Create()
    ) {}

HueStream::HueStream(ConfigPtr config, HttpClientPtr httpClient) :
    HueStream(config,
              httpClient,
              TimeManagerFactory::Create(),
              ConnectionMonitorFactory::Create(httpClient, config->GetAppSettings()),
              MixerFactory::Create(),
              BridgeStorageAccessorFactory::Create(config->GetAppSettings()->GetBridgeFileName(),
                                                   config->GetBridgeSettings()),
              MessageDispatcherFactory::Create(),
              MessageTranslatorFactory::Create(config->GetAppSettings()->GetLanguage()),
              ConnectorFactory::Create(config),
              GroupControllerFactory::Create(httpClient)
    ) {}

HueStream::HueStream(ConfigPtr config,
                     HttpClientPtr httpClient,
                     TimeManagerPtr timeManager,
                     ConnectionMonitorPtr connectionMonitor,
                     MixerPtr engine,
                     BridgeStorageAccessorPtr storageAccessor,
                     MessageDispatcherPtr dispatcher,
                     MessageTranslatorPtr translator,
                     ConnectorPtr connector,
                     BasicGroupLightControllerPtr groupController):
    HueStream(config,
              std::move(httpClient),
              timeManager,
              std::move(connectionMonitor),
              std::move(engine),
              std::move(storageAccessor),
              std::move(dispatcher),
              std::move(translator),
              connector,
              std::move(groupController),
              HueStreamFactory::Create(config->GetStreamSettings(), config->GetAppSettings(), timeManager, connector)) {
    }

HueStream::HueStream(ConfigPtr config,
                     HttpClientPtr httpClient,
                     TimeManagerPtr timeManager,
                     ConnectionMonitorPtr connectionMonitor,
                     MixerPtr engine,
                     BridgeStorageAccessorPtr storageAccessor,
                     MessageDispatcherPtr dispatcher,
                     MessageTranslatorPtr translator,
                     ConnectorPtr connector,
                     BasicGroupLightControllerPtr groupController,
                     StreamPtr stream):
    HueStream(config,
              httpClient,
              timeManager,
              connectionMonitor,
              engine,
              storageAccessor,
              dispatcher,
              translator,
              connector,
              groupController,
              stream,
              ConnectFactory::Create(httpClient, dispatcher, config->GetBridgeSettings(), config->GetAppSettings(), stream, storageAccessor)) {
}

HueStream::HueStream(ConfigPtr config,
                     HttpClientPtr httpClient,
                     TimeManagerPtr timeManager,
                     ConnectionMonitorPtr connectionMonitor,
                     MixerPtr mixer,
                     BridgeStorageAccessorPtr storageAccessor,
                     MessageDispatcherPtr dispatcher,
                     MessageTranslatorPtr translator,
                     ConnectorPtr connector,
                     BasicGroupLightControllerPtr groupController,
                     StreamPtr stream,
                     ConnectPtr connect):
    
    _config(std::move(config)),
    _httpClient(std::move(httpClient)),
    _connectionMonitor(std::move(connectionMonitor)),
    _activeBridge(std::make_shared<Bridge>(_config->GetBridgeSettings())),
    _streamRenderCallback(std::bind(&HueStream::Render, this)), 
    _storageAccessor(std::move(storageAccessor)),
    _dispatcher(std::move(dispatcher)),
    _translator(std::move(translator)),
    _timeManager(std::move(timeManager)),
    _connector(std::move(connector)),
    _groupController(std::move(groupController)),
    _stream(std::move(stream)),
    _connect(std::move(connect)),
    _mixer(std::move(mixer)) {
    FeedbackMessage::SetTranslator(_translator);
    TimeProviderProvider::SetProviderMethod([this]() -> TimeProviderPtr {return _timeManager;});
    Serializable::SetObjectBuilder(std::make_shared<ObjectBuilder>(_config->GetBridgeSettings()));

    _connectionMonitor->SetFeedbackMessageCallback([this](const FeedbackMessage &message) {
        _connect->OnBridgeMonitorEvent(message);
    });

    _connect->SetFeedbackMessageCallback([this](const FeedbackMessage &message) {
        HueStream::NewFeedbackMessage(message);
    });

    _dispatcher->Execute();

    _stream->SetRenderCallback(_streamRenderCallback);
}

HueStream::~HueStream() {
}

ConfigPtr HueStream::GetConfig() {
    return _config;
}

void HueStream::ConnectManualBridgeInfo(BridgePtr bridge) {
    _connect->SetManual(std::move(bridge));
    _connect->WaitUntilReady();
}

void HueStream::ConnectManualBridgeInfoAsync(BridgePtr bridge) {
    _connect->SetManual(std::move(bridge));
}

void HueStream::ResetBridgeInfo() {
    _connect->ResetBridge();
    _connect->WaitUntilReady();
}

void HueStream::ResetBridgeInfoAsync() {
    _connect->ResetBridge();
}

void HueStream::ResetAllPersistentData() {
    _connect->ResetAllData();
    _connect->WaitUntilReady();
}

void HueStream::ResetAllPersistentDataAsync() {
    _connect->ResetAllData();
}

void HueStream::LoadBridgeInfo() {
    _connect->Load();
    _connect->WaitUntilReady();
}

void HueStream::LoadBridgeInfoAsync() {
    _connect->Load();
}

void HueStream::ConnectBridge() {
    _connect->ConnectToBridge();
    _connect->WaitUntilReady();
}

void HueStream::ConnectBridgeAsync() {
    _connect->ConnectToBridge();
}

void HueStream::ConnectBridgeBackground() {
    _connect->ConnectToBridgeBackground();
    _connect->WaitUntilReady();
}

void HueStream::ConnectBridgeBackgroundAsync() {
    _connect->ConnectToBridgeBackground();
}

void HueStream::ConnectBridgeManualIp(const std::string &ipAddress) {
    _connect->ConnectToBridgeWithIp(ipAddress);
    _connect->WaitUntilReady();
}

void HueStream::ConnectBridgeManualIpAsync(const std::string &ipAddress) {
    _connect->ConnectToBridgeWithIp(ipAddress);
}

void HueStream::ConnectNewBridge() {
    _connect->ConnectToNewBridge();
    _connect->WaitUntilReady();
}

void HueStream::ConnectNewBridgeAsync() {
    _connect->ConnectToNewBridge();
}

void HueStream::SelectGroup(std::string id) {
    _connect->SelectGroup(id);
    _connect->WaitUntilReady();
}

void HueStream::SelectGroupAsync(std::string id) {
    _connect->SelectGroup(id);
}

void HueStream::SelectGroup(GroupPtr group) {
    _connect->SelectGroup(group->GetId());
    _connect->WaitUntilReady();
}

void HueStream::SelectGroupAsync(GroupPtr group) {
    _connect->SelectGroup(group->GetId());
}

void HueStream::Start() {
    _connect->StartStream(_stream);
    _connect->WaitUntilReady();
}

void HueStream::StartAsync() {
    _connect->StartStream(_stream);
}

void HueStream::AbortConnecting() {
    _connect->Abort();
    _connect->WaitUntilReady();
}

void HueStream::AbortConnectingAsync() {
    _connect->Abort();
}

ConnectResult HueStream::GetConnectionResult() {
    return _connect->GetResult();
}

bool HueStream::IsStreamableBridgeLoaded() {
    return _activeBridge->IsReadyToStream();
}

bool HueStream::IsBridgeStreaming() {
    return _activeBridge->IsStreaming();
}

BridgePtr HueStream::GetLoadedBridge() {
    return _activeBridge;
}

BridgeStatus HueStream::GetLoadedBridgeStatus() {
    return _activeBridge->GetStatus();
}

GroupListPtr HueStream::GetLoadedBridgeGroups() {
    return _activeBridge->GetGroups();
}

void HueStream::NewFeedbackMessage(const FeedbackMessage &message) {
    _activeBridge = message.GetBridge();

    //On finishing a user trigerred event or on a bridge monitor change event
    if (message.GetId() == FeedbackMessage::ID_USERPROCEDURE_FINISHED ||
        message.GetRequestType() == FeedbackMessage::REQUEST_TYPE_INTERNAL) {
        //If bridge is streaming: mixer & stream components should be updated with new bridge data
        if (_activeBridge->IsStreaming()) {
            PrepareMixer();
        }
        //Pass new activebridge to group light controller
        _groupController->SetActiveBridge(_activeBridge);
        //Dont monitor invalid bridges
        if (!_activeBridge->IsConnectable()) {
            _connectionMonitor->Stop();
        //Poll regularly while streaming
        } else if (_activeBridge->IsStreaming()) {
            _connectionMonitor->Start(_activeBridge, _config->GetAppSettings()->GetMonitorIntervalStreamingMs());
        //Poll slowly when not streaming
        } else {
            _connectionMonitor->Start(_activeBridge, _config->GetAppSettings()->GetMonitorIntervalNotStreamingMs());
        }
    //Dont monitor when connection procedure is ongoing
    } else if (message.GetId() == FeedbackMessage::ID_USERPROCEDURE_STARTED) {
        _connectionMonitor->Stop();
    }

    //Forward message to registered client
    if (_handler != nullptr) {
        _handler->NewFeedbackMessage(message);
    }
    if (_callback != nullptr) {
        _callback(message);
    }
}

void HueStream::AddEffect(EffectPtr newEffect) {
    _mixer->AddEffect(newEffect);
}

void HueStream::AddLightScript(LightScriptPtr script) {
    auto actions = script->GetActions();
    for (const auto &action : *actions) {
        _mixer->AddEffect(action);
    }
}

EffectPtr HueStream::GetEffectByName(const std::string &name) {
    return _mixer->GetEffectByName(name);
}

void HueStream::RegisterFeedbackHandler(FeedbackMessageHandlerPtr handler) {
    _handler = std::move(handler);
}

void HueStream::RegisterFeedbackCallback(FeedbackMessageCallback callback) {
    _callback = std::move(callback);
}

void HueStream::RenderSingleFrame() {
    _stream->SetRenderCallback(_streamRenderCallback);
    _stream->RenderSingleFrame();
}

void HueStream::Stop() {
    if (!_activeBridge->IsStreaming()) {
        return;
    }

    _connect->StopStream(_stream);
    _connect->WaitUntilReady();
}

void HueStream::StopAsync() {
    if (!_activeBridge->IsStreaming()) {
        return;
    }

    _connect->StopStream(_stream);
}


void HueStream::ShutDown() {
    AbortConnecting();
    Stop();
    _connectionMonitor->Stop();
    _dispatcher->Stop();
}

void HueStream::LockMixer() {
    _mixer->Lock();
}

void HueStream::UnlockMixer() {
    _mixer->Unlock();
}

void HueStream::Render() {
    _mixer->Lock();
    _mixer->Render();

    if (_lightStateChangedHandler != nullptr && _activeBridge->IsValidGroupSelected()) {
        _lightStateChangedHandler->OnLightStateUpdated(clone_list(_activeBridge->GetGroup()->GetLights()));
    }

    _mixer->Unlock();
}

void HueStream::PrepareMixer() {
    _mixer->Lock();
    _mixer->SetGroup(_activeBridge->GetGroup());
    _stream->UpdateBridgeGroup(_activeBridge);
    _mixer->Unlock();
    _stream->SetRenderCallback(_streamRenderCallback);
}

void HueStream::RegisterLightStateUpdatedHandler(LightStateChangedHandlerPtr handler) {
    _lightStateChangedHandler = std::move(handler);
}

int32_t HueStream::GetStreamCounter() {
    return _stream->GetStreamCounter();
}

void HueStream::ChangeStreamingMode(StreamingMode mode) {
    if (_config->GetStreamingMode() == mode) {
        return;
    }
    _config->SetStreamingMode(mode);
    _connector = ConnectorFactory::Create(_config);
}

TimeManagerPtr HueStream::GetTimeManager() {
    return _timeManager;
}

void HueStream::SetGroupOn(bool on) {
    _groupController->SetOn(on);
}

void HueStream::SetGroupBrightness(double brightness) {
    _groupController->SetBrightness(brightness);
}

void HueStream::SetGroupColor(double x, double y) {
    _groupController->SetColor(x, y);
}

void HueStream::SetGroupPreset(BasicGroupLightController::LightPreset preset, bool excludeLightsWhichAreOff) {
    _groupController->SetPreset(preset);
}

void HueStream::SetGroupPreset(double brightness, double x, double y, bool excludeLightsWhichAreOff) {
    _groupController->SetPreset(brightness, x, y);
}

void HueStream::SetGroupScene(const std::string &sceneTag) {
    _groupController->SetScene(sceneTag);
}

}  // namespace huestream
