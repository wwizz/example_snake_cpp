/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_HUESTREAM_H_
#define HUESTREAM_HUESTREAM_H_

#include <huestream/config/Config.h>
#include <huestream/common/data/Bridge.h>
#include <huestream/common/data/Group.h>
#include <huestream/effect/effects/base/Effect.h>
#include <huestream/connect/FeedbackMessage.h>
#include <huestream/common/serialize/SerializerHelper.h>
#include <huestream/common/time/ITimeManager.h>
#include <huestream/common/language/IMessageTranslator.h>
#include <huestream/connect/Connect.h>
#include <huestream/connect/IBridgeStorageAccessor.h>
#include <huestream/connect/IMessageDispatcher.h>
#include <huestream/connect/ConnectionMonitor.h>
#include <huestream/connect/IBasicGroupLightController.h>
#include <huestream/effect/Mixer.h>
#include <huestream/stream/Stream.h>
#include <huestream/connect/ConnectionFlow.h>
#include <huestream/connect/IFeedbackMessageHandler.h>
#include <huestream/effect/lightscript/LightScript.h>
#include <huestream/HueStreamFactories.h>
#include <util/Factory.h>

#include <edtls/wrapper/mbedtls/EntropyProviderBase.h>

#include <memory>
#include <string>
#include "connect/BasicGroupLightController.h"

namespace huestream {

typedef std::function<void(const FeedbackMessage &message)> FeedbackMessageCallback;

class Config;
typedef std::shared_ptr<Config> ConfigPtr;

class IConnect;
typedef std::shared_ptr<IConnect> ConnectPtr;

class HueStream;
typedef shared_ptr<HueStream> HueStreamPtr;

typedef huesdk_lib::Factory<huestream::BridgeStorageAccessorPtr, const std::string&, huestream::BridgeSettingsPtr> BridgeStorageAccessorFactory;
typedef huesdk_lib::Factory<huestream::MixerPtr> MixerFactory;
typedef huesdk_lib::Factory<huestream::MessageTranslatorPtr, std::string> MessageTranslatorFactory;
typedef huesdk_lib::Factory<EntropyProviderBasePtr> EntropyProviderFactory;
typedef huesdk_lib::Factory<ConnectPtr, HttpClientPtr, MessageDispatcherPtr, BridgeSettingsPtr, AppSettingsPtr, StreamPtr, BridgeStorageAccessorPtr> ConnectFactory;
typedef huesdk_lib::Factory<StreamPtr, StreamSettingsPtr, AppSettingsPtr, TimeManagerPtr, ConnectorPtr> HueStreamFactory;
typedef huesdk_lib::Factory<HttpClientPtr> HttpClientFactory;
typedef huesdk_lib::Factory<ConnectionMonitorPtr, HttpClientPtr, AppSettingsPtr> ConnectionMonitorFactory;
typedef huesdk_lib::Factory<huestream::MixerPtr> MixerFactory;
typedef huesdk_lib::Factory<huestream::TimeManagerPtr> TimeManagerFactory;
typedef huesdk_lib::Factory<huestream::MessageDispatcherPtr> MessageDispatcherFactory;
typedef huesdk_lib::Factory<huestream::ConnectorPtr, ConfigPtr> ConnectorFactory;
typedef huesdk_lib::Factory<huestream::BasicGroupLightControllerPtr, HttpClientPtr> GroupControllerFactory;


class ILightStateChangedHandler {
 public:
    ILightStateChangedHandler() = default;
    virtual ~ILightStateChangedHandler() = default;

    virtual void OnLightStateUpdated(LightListPtr lights) = 0;
};

typedef std::shared_ptr<ILightStateChangedHandler> LightStateChangedHandlerPtr;
typedef std::shared_ptr<IFeedbackMessageHandler> FeedbackMessageHandlerPtr;

/**
 main huestream library facade which aggregates the connect, stream and effect components
 */
class HueStream {
 public:
    /**
     constructor
     @param config Reference to object containing the library configuration parameters
     */
    explicit HueStream(ConfigPtr config);

    /**
     constructor used for injecting alternative implementations for classes used by HueStream facade
     */
    HueStream(ConfigPtr config,
              HttpClientPtr httpClient);



    /**
     destructor
     */
    virtual ~HueStream();

    /**
     Get the current config
     */
    ConfigPtr GetConfig();


    /**
     set object which handles feedback from the library
     @param handler Reference to object which handles feedback from the library
     */
    virtual void RegisterFeedbackHandler(FeedbackMessageHandlerPtr handler);

    /**
     set function which handles feedback from the library
     @param callback Function which handles feedback from the library
     */
    virtual void RegisterFeedbackCallback(FeedbackMessageCallback callback);

    /**
     set object which handles light state update events from the library. HueStream will call OnLightStateUpdate(  )
     method of this object *every time* light state of the active group changes.
     Use this method to render the output light states.
     @param handler Reference to object which handles feedback from the library
     */
    virtual void RegisterLightStateUpdatedHandler(LightStateChangedHandlerPtr handler);

    /**
     if a valid bridge is stored connect to it, else discover a new bridge and connect to it (blocking execution)
     */
    virtual void ConnectBridge();

    /**
     if a valid bridge is stored connect to it, else discover a new bridge and connect to it (non blocking execution)
     */
    virtual void ConnectBridgeAsync();

    /**
     if a valid bridge is stored connect to it, else check if a valid bridge is on the network but don't connect yet (blocking execution)
     */
    virtual void ConnectBridgeBackground();

    /**
     if a valid bridge is stored connect to it, else check if a valid bridge is on the network but don't connect yet (non blocking execution)
     */
    virtual void ConnectBridgeBackgroundAsync();

    /**
     connect to a bridge with manually set ip address (blocking execution)
     @param ipAddress Ip address of the bridge to connect to
     */
    virtual void ConnectBridgeManualIp(const std::string &ipAddress);

    /**
     connect to a bridge with manually set ip address (non blocking execution)
     @param ipAddress Ip address of the bridge to connect to
     */
    virtual void ConnectBridgeManualIpAsync(const std::string &ipAddress);

    /**
     connect to a bridge with manually set ip address, username and clientkey (blocking execution)
     @param bridge Bridge object with a valid ip address, username and clientkey set
     */
    virtual void ConnectManualBridgeInfo(BridgePtr bridge);

    /**
     connect to a bridge with manually set ip address, username and clientkey (non blocking execution)
     @param bridge Bridge object with a valid ip address, username and clientkey set
     */
    virtual void ConnectManualBridgeInfoAsync(BridgePtr bridge);

    /**
     connect to a new bridge, even if a valid bridge is already active (blocking execution)
     */
    virtual void ConnectNewBridge();

    /**
     connect to a new bridge, even if a valid bridge is already active (non blocking execution)
     */
    virtual void ConnectNewBridgeAsync();

    /**
     select entertainment group to use (blocking execution)
     @param group Entertainment group to use
     */
    virtual void SelectGroup(GroupPtr group);

    /**
     select entertainment group to use (non blocking execution)
     @param group Entertainment group to use
     */
    virtual void SelectGroupAsync(GroupPtr group);

    /**
     select entertainment group to use by id (blocking execution)
     @param id Id of the entertainment group to use
     */
    virtual void SelectGroup(std::string id);

    /**
     select entertainment group to use by id (non blocking execution)
     @param id Id of the entertainment group to use
     */
    virtual void SelectGroupAsync(std::string id);

    /**
     start streaming (blocking execution)
     */
    virtual void Start();

    /**
     start streaming (non blocking execution)
     */
    virtual void StartAsync();

    /**
     delete stored information of current bridge (blocking execution)
     */
    virtual void ResetBridgeInfo();

    /**
     delete stored information of current bridge (non blocking execution)
     */
    virtual void ResetBridgeInfoAsync();

    /**
     delete all stored bridge information (blocking execution)
     */
    virtual void ResetAllPersistentData();

    /**
     delete all stored bridge information (non blocking execution)
     */
    virtual void ResetAllPersistentDataAsync();

    /**
     load stored bridge information (blocking execution)
     */
    virtual void LoadBridgeInfo();

    /**
     load stored bridge information (non blocking execution)
     */
    virtual void LoadBridgeInfoAsync();

    /**
     abort connection procedure (blocking execution)
     */
    virtual void AbortConnecting();

    /**
     abort connection procedure (non blocking execution)
     */
    virtual void AbortConnectingAsync();

    /**
     get the result of the last connection procedure
     @return result of the last connection procedure
     */
    virtual ConnectResult GetConnectionResult();

    /**
     check if the currently loaded bridged is correctly configured for streaming
     @return whether a bridge is loaded which is correctly configured for streaming
     */
    virtual bool IsStreamableBridgeLoaded();

    /**
     check if the currently loaded bridged is being streamed to
     @return whether if the currently loaded bridged is being streamed to
     */
    virtual bool IsBridgeStreaming();

    /**
     get the currently loaded bridge
     @return reference to currently loaded bridge object
     */
    virtual BridgePtr GetLoadedBridge();

    /**
     get the status of the currently loaded bridge
     @return status of the currently loaded bridge
     */
    virtual BridgeStatus GetLoadedBridgeStatus();

    /**
     get the list of entertainment groups on the currently loaded bridge
     @return list of entertainment groups on the currently loaded bridge
     */
    virtual GroupListPtr GetLoadedBridgeGroups();

    /**
     render a single frame
     @note only required if useRenderThread option is disabled
     */
    virtual void RenderSingleFrame();

    /**
     stop streaming (blocking execution)
     */
    virtual void Stop();

    /**
     stop streaming (non blocking execution)
     */
    virtual void StopAsync();

    /**
     lock the engine to be able to add and/or modify effects
     @note required if useRenderThread option is enabled
     */
    virtual void LockMixer();

    /**
     unlock the engine for rendering
     @note required if useRenderThread option is enabled
     */
    virtual void UnlockMixer();

    /**
     add a single effect to the engine
     @param newEffect Reference to effect to be added
     */
    virtual void AddEffect(EffectPtr newEffect);

    /**
     add a full light script to the engine
     @param script Reference to light script to be added
     */
    virtual void AddLightScript(LightScriptPtr script);

    /**
     get effect by name
     @param name Name of the effect to retrieve
     @return Reference to effect if found, else nullptr
     */
    virtual EffectPtr GetEffectByName(const std::string &name);

    /**
     shut down the huestream module
     */
    virtual void ShutDown();

    /**
     prepare the render engine for streaming
     @note mostly internal use: is automatically called as part of starting the stream
     */
    virtual void PrepareMixer();


    /**
     Get the number of frames sent
     @note mostly internal use
     */
    virtual int32_t GetStreamCounter();

    /**
     Change the streaming mode
     @note mostly internal use
     */
    virtual void ChangeStreamingMode(StreamingMode mode);

    /**
     Set the on state of selected entertainment group
     @note this only works when not streaming and a valid group is selected
     */
    virtual void SetGroupOn(bool on);

    /**
     Set the brightness of selected entertainment group
     @param brightness Relative brightness between 0 and 1
     @note this only works when not streaming and a valid group is selected
     @note do not update value more than twice per second
     */
    virtual void SetGroupBrightness(double brightness);

    /**
     Set the color of selected entertainment group
     @param x X-coordinate in CIE XY color space
     @param y Y-coordinate in CIE XY color space
     @note this only works when not streaming and a valid group is selected
     @note do not update value more than twice per second
     */
    virtual void SetGroupColor(double x, double y);

    /**
     Set a default light preset on the selected entertainment group
     @param preset One of the Hue specified presets
     @note this only works when not streaming and a valid group is selected
     */
    virtual void SetGroupPreset(BasicGroupLightController::LightPreset preset, bool excludeLightsWhichAreOff = false);

    /**
     Set a custom light preset on the selected entertainment group
     @param brightness Relative brightness between 0 and 1
     @param x X-coordinate in CIE XY color space
     @param y Y-coordinate in CIE XY color space
     @note this only works when not streaming and a valid group is selected
     */
    virtual void SetGroupPreset(double brightness, double x, double y, bool excludeLightsWhichAreOff = false);

    /**
     Set the scene of selected entertainment group
     @note this only works when not streaming and a valid group is selected
     */
    virtual void SetGroupScene(const std::string &sceneTag);

    virtual TimeManagerPtr GetTimeManager();

 protected:
    HueStream(ConfigPtr config,
              HttpClientPtr httpClient,
              TimeManagerPtr timeManager,
              ConnectionMonitorPtr connectionMonitor,
              MixerPtr engine,
              BridgeStorageAccessorPtr storageAccessor,
              MessageDispatcherPtr dispatcher,
              MessageTranslatorPtr translator,
              ConnectorPtr connector,
              BasicGroupLightControllerPtr groupController);

    HueStream(ConfigPtr config,
              HttpClientPtr httpClient,
              TimeManagerPtr timeManager,
              ConnectionMonitorPtr connectionMonitor,
              MixerPtr engine,
              BridgeStorageAccessorPtr storageAccessor,
              MessageDispatcherPtr dispatcher,
              MessageTranslatorPtr translator,
              ConnectorPtr connector,
              BasicGroupLightControllerPtr groupController,
              StreamPtr stream);

    HueStream(ConfigPtr config,
              HttpClientPtr httpClient,
              TimeManagerPtr timeManager,
              ConnectionMonitorPtr connectionMonitor,
              MixerPtr engine,
              BridgeStorageAccessorPtr storageAccessor,
              MessageDispatcherPtr dispatcher,
              MessageTranslatorPtr translator,
              ConnectorPtr connector,
              BasicGroupLightControllerPtr groupController,
              StreamPtr stream,
              ConnectPtr hueConnect);

    virtual void NewFeedbackMessage(const FeedbackMessage &message);
    virtual void Render();

    ConfigPtr _config;
    static HueStreamPtr _hueStream;

    FeedbackMessageHandlerPtr _handler;
    FeedbackMessageCallback _callback;
    HttpClientPtr _httpClient;
    ConnectionMonitorPtr _connectionMonitor;
    BasicGroupLightControllerPtr _groupController;

 PROP_DEFINE(HueStream, BridgePtr, activeBridge, ActiveBridge);
 PROP_DEFINE(HueStream, StreamRenderCallback, streamRenderCallback, StreamRenderCallback);

 private:
    BridgeStorageAccessorPtr _storageAccessor;
    MessageDispatcherPtr _dispatcher;
    MessageTranslatorPtr _translator;
    LightStateChangedHandlerPtr _lightStateChangedHandler;
    TimeManagerPtr _timeManager;
    ConnectorPtr _connector;
    StreamPtr _stream;
    ConnectPtr _connect;
    MixerPtr _mixer;
};

}  // namespace huestream

#endif  // HUESTREAM_HUESTREAM_H_
