/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/Bridge.h>
#include <huestream/common/data/ApiVersion.h>
#include <huestream/config/Config.h>

#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

namespace huestream {

#define NO_SELECTED_GROUP "___NO_SELECTED_GROUP___"

std::map<BridgeStatus, std::string> Bridge::statusTagMap = {
    {BRIDGE_EMPTY,                  "ID_NO_BRIDGE_FOUND"},
    {BRIDGE_NOT_FOUND,              "ID_NO_BRIDGE_FOUND"},
    {BRIDGE_INVALID_MODEL,          "ID_INVALID_MODEL"},
    {BRIDGE_NOT_CONNECTABLE,        "ID_BRIDGE_NOT_FOUND"},
    {BRIDGE_NOT_CONNECTED,          "ID_BRIDGE_NOT_FOUND"},
    {BRIDGE_INVALID_VERSION,        "ID_INVALID_VERSION"},
    {BRIDGE_INVALID_CLIENTKEY,      "BRIDGE_INVALID_CLIENTKEY"},
    {BRIDGE_NO_GROUP_AVAILABLE,     "ID_NO_GROUP_AVAILABLE"},
    {BRIDGE_INVALID_GROUP_SELECTED, "ID_SELECT_GROUP"},
    {BRIDGE_BUSY,                   "ID_BRIDGE_BUSY"},
    {BRIDGE_READY,                  "BRIDGE_READY"},
    {BRIDGE_STREAMING,              "BRIDGE_STREAMING" }
};

PROP_IMPL(Bridge, BridgeSettingsPtr, bridgeSettings, BridgeSettings);
PROP_IMPL(Bridge, std::string, name, Name);
PROP_IMPL(Bridge, std::string, modelId, ModelId);
PROP_IMPL(Bridge, std::string, apiversion, Apiversion);
PROP_IMPL(Bridge, std::string, id, Id);
PROP_IMPL(Bridge, std::string, ipAddress, IpAddress);
PROP_IMPL(Bridge, std::string, tcpPort, TcpPort);
PROP_IMPL(Bridge, std::string, sslPort, SslPort);
PROP_IMPL_BOOL(Bridge, bool, isValidIp, IsValidIp);
PROP_IMPL_BOOL(Bridge, bool, isAuthorized, IsAuthorized);
PROP_IMPL_BOOL(Bridge, bool, isBusy, IsBusy);
PROP_IMPL(Bridge, std::string, clientKey, ClientKey);
PROP_IMPL(Bridge, std::string, user, User);
PROP_IMPL(Bridge, GroupListPtr, groups, Groups);
PROP_IMPL(Bridge, std::string, selectedGroup, SelectedGroup);
PROP_IMPL(Bridge, int, maxNoStreamingSessions, MaxNoStreamingSessions);
PROP_IMPL_GET(Bridge, bool, isUsingSsl, IsUsingSsl);

Bridge::Bridge(BridgeSettingsPtr bridgeSettings)
    : Bridge("", "", false, bridgeSettings) {
}

Bridge::Bridge(std::string id, std::string ip, bool ipValid, BridgeSettingsPtr bridgeSettings) :
    _bridgeSettings(bridgeSettings),
    _apiversion(""),
    _id(id),
    _ipAddress(ip),
    _isValidIp(ipValid),
    _isAuthorized(false),
    _isBusy(false),
    _groups(std::make_shared<GroupList>()),
    _selectedGroup(NO_SELECTED_GROUP),
    _maxNoStreamingSessions(0),
    _isUsingSsl(false) {
}

void Bridge::Clear() {
    SetName("");
    SetIsAuthorized(false);
    SetIsValidIp(false);
    SetIsBusy(false);
    SetUser("");
    SetId("");
    SetIpAddress("");
    SetApiversion("");
    SetModelId("");
    SetClientKey("");
    SetSelectedGroup("");
    SetGroups(std::make_shared<GroupList>());
}

BridgeStatus Bridge::GetStatus() const {
    if (IsEmpty()) {
        return BRIDGE_EMPTY;
    }
    if (!IsFound()) {
        return BRIDGE_NOT_FOUND;
    }
    if (!IsValidModelId()) {
        return BRIDGE_INVALID_MODEL;
    }
    if (!IsConnectable()) {
        return BRIDGE_NOT_CONNECTABLE;
    }
    if (!IsConnected()) {
        return BRIDGE_NOT_CONNECTED;
    }
    if (!IsValidApiVersion()) {
        return BRIDGE_INVALID_VERSION;
    }
    if (!IsValidClientKey()) {
        return BRIDGE_INVALID_CLIENTKEY;
    }
    if (GetGroups()->size() == 0) {
        return BRIDGE_NO_GROUP_AVAILABLE;
    }
    if (!IsValidGroupSelected()) {
        return BRIDGE_INVALID_GROUP_SELECTED;
    }
    if (IsBusy()) {
        return BRIDGE_BUSY;
    }

    if (GetGroup()->Active() && GetGroup()->GetOwner() == GetUser()) {
        return BRIDGE_STREAMING;
    }

    return BRIDGE_READY;
}

std::string Bridge::GetStatusTag() const {
    auto it = statusTagMap.find(GetStatus());
    if (it != statusTagMap.end())
        return it->second;
    return "UNKNOWN";
}

std::string Bridge::GetUserStatus(MessageTranslatorPtr translator) const {
    return translator->Translate(GetStatusTag());
}

bool Bridge::IsEmpty() const {
    return _ipAddress.empty();
}

bool Bridge::IsFound() const {
    return !IsEmpty() && !_id.empty();
}

bool Bridge::IsValidModelId() const {
    std::regex re("BSB(\\d+)");
    std::smatch match;
    if (!std::regex_search(_modelId, match, re) || match.size() != 2) {
        return false;
    }
    auto version = std::stoi(match.str(1));

    return version >= _bridgeSettings->GetSupportedModel();
}

bool Bridge::IsConnectable() const {
    return IsFound() &&
        !_user.empty() &&
        IsAuthorized() &&
        IsValidModelId();
}

bool Bridge::IsConnected() const {
    return IsConnectable() && IsValidIp();
}

bool Bridge::IsValidApiVersion() const {
    ApiVersion thisVersion(_apiversion);
    ApiVersion minVersion(_bridgeSettings->GetSupportedApiVersionMajor(),
        _bridgeSettings->GetSupportedApiVersionMinor(),
        _bridgeSettings->GetSupportedApiVersionBuild());

    return thisVersion.IsValid() && thisVersion >= minVersion;
}

bool Bridge::IsValidClientKey() const {
    return _clientKey.length() == 32;
}

bool Bridge::IsSupportingHttps() const {
    return false;
}

bool Bridge::IsGroupSelected() const {
    return _selectedGroup != NO_SELECTED_GROUP;
}

bool Bridge::IsValidGroupSelected() const {
    return (GetGroup() != nullptr);
}

bool Bridge::IsProxyNodeUnreachable() const {
    auto group = GetGroup();
    if (group == nullptr) {
        return false;
    }

    return !group->GetProxyNode().isReachable;
}

bool Bridge::IsReadyToStream() const {
    return (GetStatus() == BRIDGE_READY);
}

bool Bridge::IsStreaming() const {
    return (GetStatus() == BRIDGE_STREAMING);
}

bool Bridge::HasEverBeenAuthorizedForStreaming() const {
    return IsFound() &&
        IsValidModelId() &&
        !_user.empty() &&
        IsValidClientKey();
}

bool Bridge::IsAuthorizedForStreaming() const {
    return HasEverBeenAuthorizedForStreaming() && IsAuthorized();
}

bool Bridge::SelectGroupIfOnlyOneOption() {
    if (_groups->size() == 1) {
        _selectedGroup = _groups->at(0)->GetId();
        return true;
    }
    return false;
}

bool Bridge::SelectGroup(std::string id) {
    for (auto i = _groups->begin(); i != _groups->end(); ++i) {
        if ((*i)->GetId() == id) {
            SetSelectedGroup(id);
            return true;
        }
    }
    return false;
}

GroupPtr Bridge::GetGroup() const {
    return GetGroupById(_selectedGroup);
}

LightListPtr Bridge::GetGroupLights() const {
    auto group = GetGroup();
    if (group == nullptr) {
        return std::make_shared<LightList>();
    }
    return group->GetLights();
}

GroupPtr Bridge::GetGroupById(std::string id) const {
    for (auto i = _groups->begin(); i != _groups->end(); ++i) {
        if ((*i)->GetId() == id) {
            return (*i);
        }
    }
    return nullptr;
}

void Bridge::DeleteGroup(std::string id) {
    for (auto i = _groups->begin(); i != _groups->end(); ++i) {
        if ((*i)->GetId() == id) {
            _groups->erase(i);
            break;
        }
    }
}

GroupListPtr Bridge::GetGroupsOwnedByOtherClient() const {
    auto groups = std::make_shared<GroupList>();
    for (auto i = _groups->begin(); i != _groups->end(); ++i) {
        if ((*i)->Active() && (*i)->GetOwner() != _user) {
            groups->push_back(*i);
        }
    }
    return groups;
}

int Bridge::GetCurrentNoStreamingSessions() const {
    int sessions = 0;
    for (auto i = _groups->begin(); i != _groups->end(); ++i) {
        if ((*i)->Active()) {
            sessions++;
        }
    }
    return sessions;
}

std::ostringstream Bridge::GetStreamApiRootUrl() const {
    std::ostringstream oss;

    auto protocol = _isUsingSsl ? "https" : "http";
    oss << protocol << "://" << _ipAddress;

    if (_isUsingSsl) {
        if (!_sslPort.empty()) {
            oss << ":" << _sslPort;
        }
    } else {
        if (!_tcpPort.empty()) {
            oss << ":" << _tcpPort;
        }
    }

    oss << "/api/";

    return oss;
}

std::string Bridge::GetApiRootUrl() const {
    return GetStreamApiRootUrl().str();
}

std::string Bridge::GetBaseUrl() const {
    auto oss = GetStreamApiRootUrl();
    if (!_user.empty()) {
        oss << _user << "/";
    }

    return oss.str();
}

std::string Bridge::GetSmallConfigUrl() const {
    auto oss = GetStreamApiRootUrl();

    oss << "config/";

    return oss.str();
}

std::string Bridge::GetSelectedGroupUrl() const {
    auto oss = GetStreamApiRootUrl();
    oss << _user << "/" << "groups/" << _selectedGroup;

    return oss.str();
}

std::string Bridge::GetSelectedGroupActionUrl() const {
    auto oss = GetStreamApiRootUrl();
    oss << _user << "/" << "groups/" << _selectedGroup << "/" << "action";

    return oss.str();
}

void Bridge::Serialize(JSONNode *node) const {
    SerializeBase(node);
    SerializeList(node, AttributeGroups, _groups);
}

void Bridge::Deserialize(JSONNode *node) {
    DeserializeBase(node);
    DeserializeList<GroupListPtr, Group>(node, &_groups, AttributeGroups);
}

std::string Bridge::GetTypeName() const {
    return type;
}

std::string Bridge::SerializeCompact() const {
    JSONNode n;
    SerializeBase(&n);
    return n.write();
}

void Bridge::DeserializeCompact(std::string jsonString) {
    auto n = libjson::parse(jsonString);
    DeserializeBase(&n);
}

void Bridge::SerializeBase(JSONNode *node) const {
    Serializable::Serialize(node);

    SerializeValue(node, AttributeName, _name);
    SerializeValue(node, AttributeModelId, _modelId);
    SerializeValue(node, AttributeApiversion, _apiversion);
    SerializeValue(node, AttributeId, _id);
    SerializeValue(node, AttributeIpAddress, _ipAddress);
    SerializeValue(node, AttributeTcpPort, _tcpPort);
    SerializeValue(node, AttributeSslPort, _sslPort);
    SerializeValue(node, AttributeIsValidIp, _isValidIp);
    SerializeValue(node, AttributeIsAuthorized, _isAuthorized);
    SerializeValue(node, AttributeClientKey, _clientKey);
    SerializeValue(node, AttributeUser, _user);
    SerializeValue(node, AttributeSelectedGroup, _selectedGroup);
    SerializeValue(node, AttributeMaxNoStreamingSessions, _maxNoStreamingSessions);
}

void Bridge::DeserializeBase(JSONNode *node) {
    Serializable::Deserialize(node);

    DeserializeValue(node, AttributeName, &_name, "");
    DeserializeValue(node, AttributeModelId, &_modelId, "");
    DeserializeValue(node, AttributeApiversion, &_apiversion, "");
    DeserializeValue(node, AttributeId, &_id, "");
    DeserializeValue(node, AttributeIpAddress, &_ipAddress, "");
    DeserializeValue(node, AttributeTcpPort, &_tcpPort, "");
    DeserializeValue(node, AttributeSslPort, &_sslPort, "");
    DeserializeValue(node, AttributeIsValidIp, &_isValidIp, false);
    DeserializeValue(node, AttributeIsAuthorized, &_isAuthorized, false);
    DeserializeValue(node, AttributeClientKey, &_clientKey, "");
    DeserializeValue(node, AttributeUser, &_user, "");
    DeserializeValue(node, AttributeSelectedGroup, &_selectedGroup, "");
    DeserializeValue(node, AttributeMaxNoStreamingSessions, &_maxNoStreamingSessions, 0);
}

BridgePtr Bridge::Clone() const {
    auto bridgeCopy = std::make_shared<Bridge>(*this);
    bridgeCopy->SetGroups(clone_list(_groups));
    return bridgeCopy;
}

}  // namespace huestream
