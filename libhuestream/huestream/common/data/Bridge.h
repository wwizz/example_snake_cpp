/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
/** @file */

#ifndef HUESTREAM_COMMON_DATA_BRIDGE_H_
#define HUESTREAM_COMMON_DATA_BRIDGE_H_

#include <huestream/common/serialize/SerializerHelper.h>
#include <huestream/common/language/IMessageTranslator.h>
#include <huestream/common/data/BridgeSettings.h>
#include <huestream/common/data/Group.h>

#include <map>
#include <string>
#include <sstream>

namespace huestream {

/**
 status of the bridge
 */
enum BridgeStatus {
    BRIDGE_EMPTY,                   ///< empty bridge object
    BRIDGE_NOT_FOUND,               ///< bridge not found
    BRIDGE_INVALID_MODEL,           ///< bridge hardware model does not support streaming
    BRIDGE_NOT_CONNECTABLE,         ///< bridge cannot be connected to because of invalid ip or authorization
    BRIDGE_NOT_CONNECTED,           ///< bridge is not connected
    BRIDGE_INVALID_VERSION,         ///< bridge software version does not support streaming
    BRIDGE_INVALID_CLIENTKEY,       ///< bridge clientkey is invalid
    BRIDGE_NO_GROUP_AVAILABLE,      ///< bridge has no entertainment group configured
    BRIDGE_INVALID_GROUP_SELECTED,  ///< no valid entertainment group is selected yet
    BRIDGE_BUSY,                    ///< bridge is busy streaming by another client
    BRIDGE_READY,                   ///< bridge is ready to stream
    BRIDGE_STREAMING                ///< bridge is streaming
};

/**
 Hue bridge
 */
class Bridge : public Serializable {
 public:
    static constexpr const char* type = "huestream.Bridge";

    static std::map<BridgeStatus, std::string> statusTagMap;

    explicit Bridge(BridgeSettingsPtr bridgeSettings);
    Bridge(std::string id, std::string ip, bool ipValid, BridgeSettingsPtr bridgeSettings);

    virtual ~Bridge() {}

    /**
     empty the bridge object
     */
    void Clear();

    void Serialize(JSONNode *node) const override;

    void Deserialize(JSONNode *node) override;

    std::string SerializeCompact() const;

    void DeserializeCompact(std::string jsonString);

    std::string GetTypeName() const override;

    /**
     get current status of bridge
     */
    virtual BridgeStatus GetStatus() const;

    /**
     get tag or 'string id' of the current bridge status used for user message translation lookup
     */
    virtual std::string GetStatusTag() const;

    /**
     get user message string describing action required from user given current bridge status
     */
    virtual std::string GetUserStatus(MessageTranslatorPtr translator) const;

    /**
     * @return API url of the bridge (examples: "http://127.0.0.1/api/" or "http://127.0.0.1:4200/api/")
     */
    std::string GetApiRootUrl() const;

    /**
     get bridge uri
     */
    virtual std::string GetBaseUrl() const;

    /**
     get uri for selected entertainment group
     */
    virtual std::string GetSelectedGroupUrl() const;

    /**
     get uri for sending action to selected entertainment group
     */
    virtual std::string GetSelectedGroupActionUrl() const;

    /**
     check whether this bridge is currently correctly configured to be streamed to
     */
    virtual bool IsReadyToStream() const;

    /**
     check whether this bridge is currently being streamed to
     */
    virtual bool IsStreaming() const;

    /**
     check if this is an empty bridge object
     */
    virtual bool IsEmpty() const;

    /**
     check if this is a discovered bridge object
     */
    virtual bool IsFound() const;

    /**
     check if this bridge has a valid ip and authorization to connect to
     */
    virtual bool IsConnectable() const;

    /**
      check if this bridge is connected
      */
    virtual bool IsConnected() const;

    /**
     check if this bridge hardware model support streaming
     */
    virtual  bool IsValidModelId() const;

    /**
     check if this bridge software version supports streaming
     */
    virtual bool IsValidApiVersion() const;

    /**
     check if this bridge has a valid client key
     */
    virtual bool IsValidClientKey() const;

    /**
     check if this bridge has ever received valid authorization for streaming
     */
    virtual bool HasEverBeenAuthorizedForStreaming() const;

    /**
     check if this bridge currently has valid authorization for streaming
     */
    virtual bool IsAuthorizedForStreaming() const;

    /**
     select entertainment group if there is only one available, else do nothing
     @return whether an entertainment group has been selected
     */
    virtual bool SelectGroupIfOnlyOneOption();

    /**
     select entertainment group to use for streaming
     @param id Id of the entertainment group to select
     */
    virtual bool SelectGroup(std::string id);

    /**
     get currently selected entertainment group or nullptr if none is selected
     */
    virtual GroupPtr GetGroup() const;

    /**
     delete a group from list of groups in the bridge
     @note only on this bridge object not on actual bridge, mainly internal use
     @param id Id of the entertainment group to select
     */
    virtual void DeleteGroup(std::string id);

    /**
     get group which another client is currently streaming to or nullptr if there is no such group
     */
    virtual GroupPtr GetGroupOwnedByOtherClient() const;

    /**
     get number of current active streaming sessions on bridge
     */
    virtual int GetCurrentNoStreamingSessions() const;

    /**
     check if this bridge has a group selected
     */
    virtual bool IsGroupSelected() const;

    /**
     check if this bridge has a valid group selected
     */
    virtual bool IsValidGroupSelected() const;

 PROP_DEFINE(Bridge, BridgeSettingsPtr, bridgeSettings, BridgeSettings);
 PROP_DEFINE(Bridge, std::string, name, Name);
 PROP_DEFINE(Bridge, std::string, modelId, ModelId);
 PROP_DEFINE(Bridge, std::string, apiversion, Apiversion);
 PROP_DEFINE(Bridge, std::string, id, Id);
 PROP_DEFINE(Bridge, std::string, ipAddress, IpAddress);
 PROP_DEFINE(Bridge, std::string, tcpPort, TcpPort);
 PROP_DEFINE_BOOL(Bridge, bool, isValidIp, IsValidIp);
 PROP_DEFINE_BOOL(Bridge, bool, isAuthorized, IsAuthorized);
 PROP_DEFINE_BOOL(Bridge, bool, isBusy, IsBusy);
 PROP_DEFINE(Bridge, std::string, clientKey, ClientKey);
 PROP_DEFINE(Bridge, std::string, user, User);
 PROP_DEFINE(Bridge, GroupListPtr, groups, Groups);
 PROP_DEFINE(Bridge, std::string, selectedGroup, SelectedGroup);
 PROP_DEFINE(Bridge, int, maxNoStreamingSessions, MaxNoStreamingSessions);

 protected:
    virtual void SerializeBase(JSONNode *node) const;

    void DeserializeBase(JSONNode *node);

private:
    std::ostringstream GetStreamApiRootUrl() const;
};

/**
 shared pointer to a huestream::Bridge object
 */
SMART_POINTER_TYPES_FOR(Bridge)

}  // namespace huestream

#endif  // HUESTREAM_COMMON_DATA_BRIDGE_H_
