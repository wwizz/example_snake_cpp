/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/connect/FullConfigRetriever.h>
#include <huestream/common/http/IHttpClient.h>
#include <network/http/HttpRequestError.h>
#include <network/http/IHttpResponse.h>
#include <network/http/HttpRequest.h>

#include <algorithm>
#include <memory>
#include <string>

namespace huestream {

#define CLIP_ERROR_TYPE_UNAUTHORIZED_USER 1

    FullConfigRetriever::FullConfigRetriever(const HttpClientPtr http, bool useForcedActivation) :
    _http(http), _useForcedActivation(useForcedActivation), _busy(false) {
    }

    bool FullConfigRetriever::Execute(BridgePtr bridge, RetrieveCallbackHandler cb) {
        std::unique_lock<std::mutex> lk(_mutex);
        if (_busy) {
            return false;
        }
        _busy = true;

        _bridge = bridge;
        _cb = cb;
        RetrieveFullConfig();
        return true;
    }

    void FullConfigRetriever::RetrieveFullConfig() {
        const auto fullConfigUrl = _bridge->GetBaseUrl();
        _request = _http->CreateHttpRequest(fullConfigUrl);

        _request->do_get([&](const huesdk_lib::HttpRequestError &error, const huesdk_lib::IHttpResponse &response) {
            if (error.get_code() == huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS) {
                _response = response.get_body();
                ParseResponseAndExecuteCallback();
            } else if (error.get_code() != huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_CANCELED) {
                _bridge->SetIsValidIp(false);
                Finish(OPERATION_FAILED);
            }
        });
    }

    void FullConfigRetriever::ParseResponseAndExecuteCallback() {
        if (!ParseToJson()) {
            Finish(OPERATION_FAILED);
            return;
        }

        if (!CheckForNoErrorsInResponse()) {
            Finish(OPERATION_FAILED);
            return;
        }

        if (!ParseConfig()) {
            Finish(OPERATION_FAILED);
            return;
        }

        ParseGroups();

        ParseCapabilities();

        Finish(OPERATION_SUCCESS);
    }

    bool FullConfigRetriever::ParseToJson() {
        JSONNode n = libjson::parse(_response);
        if (n.type() == JSON_NULL) {
            _bridge->SetIsValidIp(false);
            return false;
        }
        _root = n;
        return true;
    }

    bool FullConfigRetriever::CheckForNoErrorsInResponse() const {
        bool hasError = false;
        bool hasUnauthorizedError = false;

        if (_root.type() == JSON_ARRAY) {
            for (auto it = _root.begin(); it != _root.end(); ++it) {
                auto entry = it->find("error");
                if (entry != it->end()) {
                    hasError = true;
                    auto i = entry->find("type");
                    if (i != entry->end()) {
                        if (i->as_int() == CLIP_ERROR_TYPE_UNAUTHORIZED_USER) {
                            hasUnauthorizedError = true;
                        }
                    }
                    break;
                }
            }
        }

        if (hasError) {
            if (hasUnauthorizedError) {
                _bridge->SetIsAuthorized(false);
                _bridge->SetIsValidIp(true);
            } else {
                _bridge->SetIsValidIp(false);
            }
            return false;
        }
        return true;
    }

    bool FullConfigRetriever::ParseConfig() const {
        if (!SerializerHelper::IsAttributeSet(&_root, "config")) {
            _bridge->SetIsValidIp(false);
            return false;
        }

        JSONNode config = (_root)["config"];
        std::string name = "";
        Serializable::DeserializeValue(&config, "name", &name, "");
        _bridge->SetName(name);

        std::string modelid = "";
        Serializable::DeserializeValue(&config, "modelid", &modelid, "");
        _bridge->SetModelId(modelid);

        std::string apiversion = "";
        Serializable::DeserializeValue(&config, "apiversion", &apiversion, "");
        _bridge->SetApiversion(apiversion);

        std::string bridgeid = "";
        Serializable::DeserializeValue(&config, "bridgeid", &bridgeid, "");
        _bridge->SetId(bridgeid);

        if (!SerializerHelper::IsAttributeSet(&_root, "groups")) {
            _bridge->SetIsValidIp(false);
            return false;
        }

        if (!SerializerHelper::IsAttributeSet(&_root, "lights")) {
            _bridge->SetIsValidIp(false);
            return false;
        }

        _bridge->SetIsValidIp(true);
        return true;
    }

    void FullConfigRetriever::ParseGroups() {
        JSONNode groups = (_root)["groups"];
        auto parsedGroups = std::make_shared<GroupList>();
        for (auto jsonGroup = groups.begin(); jsonGroup != groups.end(); ++jsonGroup) {
            if (GroupIsEntertainment(*jsonGroup)) {
                auto group = ParseEntertainmentGroup(*jsonGroup);
                parsedGroups->push_back(group);
            }
        }
        _bridge->SetGroups(parsedGroups);
    }

    bool FullConfigRetriever::GroupIsEntertainment(const JSONNode &j) {
        return (SerializerHelper::IsAttributeSet(&j, "type") && j["type"].as_string() == "Entertainment");
    }

    std::shared_ptr<Group> FullConfigRetriever::ParseEntertainmentGroup(const JSONNode &node) {
        auto groupId = node.name();

        auto group = std::make_shared<Group>();
        group->SetId(groupId);

        std::string name = "";
        Serializable::DeserializeValue(&node, "name", &name, "");
        group->SetName(name);

        ParseClass(node, group);

        ParseLightsAndLocations(node, group);

        ParseStreamActive(node, group);

        ParseGroupState(node, group);

        ParseScenes(node, group);

        return group;
    }

    void FullConfigRetriever::ParseClass(const JSONNode &node, GroupPtr group) {
        if (SerializerHelper::IsAttributeSet(&node, "class")) {
            if (node["class"].as_string() == "TV") {
                group->SetClassType(GROUPCLASS_TV);
            } else if (node["class"].as_string() == "Free") {
                group->SetClassType(GROUPCLASS_FREE);
            } else {
                group->SetClassType(GROUPCLASS_OTHER);
            }
        }
    }

    void FullConfigRetriever::ParseLightsAndLocations(const JSONNode &node, GroupPtr group) const {
        auto avgBri = 0.0;
        auto numReachableLights = 0;
        if (SerializerHelper::IsAttributeSet(&node, "locations")) {
            auto locations = node["locations"];
            for (auto i = locations.begin(); i != locations.end(); ++i) {
                auto lightId = i->name();
                auto coordinates = *i;
                auto light = GetLightInfo(i->name());
                group->AddLight(lightId, coordinates[0].as_float(), coordinates[1].as_float(),
                    light.GetName(), light.GetModel(), light.Reachable());
                if (light.Reachable()) {
                    avgBri += light.GetBrightness();
                    numReachableLights++;
                }
            }
        }
        if (numReachableLights > 0) {
            avgBri /= numReachableLights;
        }
        group->SetBrightnessState(avgBri / 254);
    }

    void FullConfigRetriever::ParseStreamActive(const JSONNode &node, GroupPtr group) {
        if (SerializerHelper::IsAttributeSet(&node, "stream")) {
            auto stream = node["stream"];
            if (SerializerHelper::IsAttributeSet(&stream, "active") &&
                SerializerHelper::IsAttributeSet(&stream, "owner")) {
                auto active = stream["active"].as_bool();
                group->SetActive(active);
                if (active) {
                    auto owner = stream["owner"].as_string();
                    group->SetOwner(owner);
                    group->SetFriendlyOwnerName(GetFriendlyName(owner));
                }
            }
        }
    }

    void FullConfigRetriever::ParseGroupState(const JSONNode &node, GroupPtr group)
    {
        if (SerializerHelper::IsAttributeSet(&node, "state")) {
            auto state = node["state"];
            if (SerializerHelper::IsAttributeSet(&state, "any_on")) {
                auto anyOn = state["any_on"].as_bool();
                group->SetOnState(anyOn);
            }
        }
    }

    std::string FullConfigRetriever::GetFriendlyName(const std::string &userName) {
        std::string name = "";
        JSONNode config = (_root)["config"];
        if (SerializerHelper::IsAttributeSet(&config, "whitelist")) {
            JSONNode whitelist = (config)["whitelist"];
            if (SerializerHelper::IsAttributeSet(&whitelist, userName)) {
                JSONNode user = (whitelist)[userName];
                Serializable::DeserializeValue(&user, "name", &name, "");
            }
        }
        return name;
    }

    LightInfo FullConfigRetriever::GetLightInfo(const std::string &id) const {
        std::string name = "";
        std::string model = "";
        auto reachable = true;
        auto on = true;
        auto bri = 0.0;

        JSONNode lights = (_root)["lights"];
        if (SerializerHelper::IsAttributeSet(&lights, id)) {
            JSONNode light = (lights)[id];
            Serializable::DeserializeValue(&light, "name", &name, "");
            Serializable::DeserializeValue(&light, "modelid", &model, "");
            if (SerializerHelper::IsAttributeSet(&light, "state")) {
                JSONNode state = (light)["state"];
                Serializable::DeserializeValue(&state, "reachable", &reachable, true);
                Serializable::DeserializeValue(&state, "on", &on, true);
                if (on) {
                    Serializable::DeserializeValue(&state, "bri", &bri, 1.0);
                }
            }
        }
        LightInfo lightInfo;
        lightInfo.SetName(name);
        lightInfo.SetModel(model);
        lightInfo.SetReachable(reachable);
        lightInfo.SetBrightness(bri);
        return lightInfo;
    }

    void FullConfigRetriever::ParseScenes(const JSONNode &node, GroupPtr group) {
        if (!SerializerHelper::IsAttributeSet(&_root, "scenes")) {
            return;
        }
        JSONNode scenes = (_root)["scenes"];
        auto parsedScenes = std::make_shared<SceneList>();
        for (auto jsonScene = scenes.begin(); jsonScene != scenes.end(); ++jsonScene) {
            if (SceneIsNotRecyclable(*jsonScene)) {
                auto scene = ParseScene(*jsonScene);
                if (scene->GetGroupId() == group->GetId()) {
                    parsedScenes->push_back(scene);
                }
            }
        }
        group->SetScenes(parsedScenes);
    }

    bool FullConfigRetriever::SceneIsNotRecyclable(const JSONNode &j) {
        return (SerializerHelper::IsAttributeSet(&j, "recycle") && j["recycle"].as_bool() == false);
    }

    ScenePtr FullConfigRetriever::ParseScene(const JSONNode &node) {
        std::string name("");
        std::string appData("");
        Serializable::DeserializeValue(&node, "name", &name, "");
        if (SerializerHelper::IsAttributeSet(&node, "appdata")) {
            JSONNode appDataNode = (node)["appdata"];
            Serializable::DeserializeValue(&appDataNode, "data", &appData, "");
        }
        return std::make_shared<Scene>(node.name(), name, appData);
    }

    void FullConfigRetriever::ParseCapabilities() const {
        if (!SerializerHelper::IsAttributeSet(&_root, "capabilities")) {
            return;
        }
        JSONNode capabilities = (_root)["capabilities"];
        if (!SerializerHelper::IsAttributeSet(&capabilities, "streaming")) {
            return;
        }
        JSONNode streaming = (capabilities)["streaming"];

        int total;
        Serializable::DeserializeValue(&streaming, "total", &total, 0);
        _bridge->SetMaxNoStreamingSessions(total);

        if (!_bridge->IsStreaming()) {
            auto selectedGroupCanBeOvertaken = _bridge->IsValidGroupSelected() && _bridge->GetGroup()->Active() && _useForcedActivation;
            int available;
            Serializable::DeserializeValue(&streaming, "available", &available, -1);
            auto busy = (available == 0) && !selectedGroupCanBeOvertaken;
            _bridge->SetIsBusy(busy);
        }
    }

    void FullConfigRetriever::Finish(OperationResult result) {
        _cb(result, _bridge);
        _busy = false;
    }

}  // namespace huestream
