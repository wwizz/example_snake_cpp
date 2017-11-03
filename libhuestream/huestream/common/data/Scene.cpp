/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/Scene.h>

#include <iostream>
#include <string>
#include <regex>

namespace huestream {

Scene::Scene() : Scene("", "", "") {
}

Scene::Scene(std::string tag, std::string name, std::string appData)
    : _tag(tag), _name(name), _appData(appData) {
}

Scene::~Scene() {
}

PROP_IMPL(Scene, std::string, tag, Tag);
PROP_IMPL(Scene, std::string, name, Name);
PROP_IMPL(Scene, std::string, appData, AppData);

std::string Scene::GetGroupId() const {
    std::string id("");
    std::regex re(".*_r(\\d+)_d.*");
    std::smatch match;

    if (std::regex_search(_appData, match, re) && match.size() >= 2) {
        auto intId = std::stoi(match.str(1));
        id = std::to_string(intId);
    }

    return id;
}

int Scene::GetDefaultSceneId() const {
    int id = -1;
    std::regex re(".*_d(\\d+)");
    std::smatch match;

    if (std::regex_search(_appData, match, re) && match.size() >= 2) {
        id = std::stoi(match.str(1));
    }

    return id;
}

void Scene::Serialize(JSONNode *node) const {
    Serializable::Serialize(node);
    SerializeValue(node, AttributeTag, _tag);
    SerializeValue(node, AttributeName, _name);
    SerializeValue(node, AttributeAppData, _appData);
}

void Scene::Deserialize(JSONNode *node) {
    Serializable::Deserialize(node);
    DeserializeValue(node, AttributeTag, &_tag, "");
    DeserializeValue(node, AttributeName, &_name, "");
    DeserializeValue(node, AttributeAppData, &_appData, "");
}

std::string Scene::GetTypeName() const {
    return type;
}

}  // namespace huestream
