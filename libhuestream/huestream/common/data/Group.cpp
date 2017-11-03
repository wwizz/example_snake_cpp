/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/Group.h>

#include <iostream>
#include <string>
#include <map>

namespace huestream {

    const std::map<GroupClass, std::string> Group::_classSerializeMap = {
        { GROUPCLASS_TV, "tv" },
        { GROUPCLASS_FREE,  "free" },
        { GROUPCLASS_OTHER, "other" }
    };

    PROP_IMPL(Group, std::string, id, Id);
    PROP_IMPL(Group, std::string, name, Name);
    PROP_IMPL(Group, GroupClass, classType, ClassType);
    PROP_IMPL(Group, LightListPtr, lights, Lights);
    PROP_IMPL_BOOL(Group, bool, active, Active);
    PROP_IMPL(Group, std::string, owner, Owner);
    PROP_IMPL(Group, std::string, friendlyOwnerName, FriendlyOwnerName);
    PROP_IMPL(Group, SceneListPtr, scenes, Scenes);
    PROP_IMPL_BOOL(Group, bool, onState, OnState);
    PROP_IMPL(Group, double, brightnessState, BrightnessState);

    Group::Group() : _id(""), _name(""), _classType(GROUPCLASS_OTHER), _lights(std::make_shared<LightList>()), _active(false), _owner(""),
                     _friendlyOwnerName(""), _scenes(std::make_shared<SceneList>()), _onState(true), _brightnessState(1.0) {
    }

    Group::~Group() {
    }

    void Group::AddLight(std::string id, double x, double y, std::string name, std::string model, bool reachable) {
        if (_lights == nullptr) {
            _lights = std::make_shared<LightList>();
        }

        auto location = Location(Clip(x, -1, 1),
                                 Clip(y, -1, 1));

        for (auto &light : *_lights) {
            if (light->GetId() == id) {
                light->SetPosition(location);
                light->SetName(name);
                light->SetModel(model);
                light->SetReachable(reachable);
                return;
            }
        }
        _lights->push_back(std::make_shared<Light>(id, location, name, model, reachable));
    }

    double Group::Clip(double value, double min, double max) const {
        if (value < min) {
            return min;
        }

        if (value > max) {
            return max;
        }

        return value;
    }

    void Group::Serialize(JSONNode *node) const {
        Serializable::Serialize(node);

        SerializeValue(node, AttributeId, _id);
        SerializeValue(node, AttributeName, _name);
        SerializeClass(node);
        SerializeList(node, AttributeLights, _lights);
        SerializeList(node, AttributeScenes, _scenes);
    }

    void Group::Deserialize(JSONNode *node) {
        Serializable::Deserialize(node);
        DeserializeValue(node, AttributeId, &_id, "");
        DeserializeValue(node, AttributeName, &_name, "");
        DeserializeClass(node);
        DeserializeList<LightListPtr, Light>(node, &_lights, AttributeLights);
        DeserializeList<SceneListPtr, Scene>(node, &_scenes, AttributeScenes);
    }

    Group* Group::Clone() {
        auto g = new Group();
        g->_id = _id;
        g->_name = _name;
        g->_classType = _classType;
        g->_lights = clone_list(_lights);
        return g;
    }

    std::string Group::GetTypeName() const {
        return type;
    }

    void Group::SerializeClass(JSONNode *node) const {
        auto it = _classSerializeMap.find(_classType);
        if (it != _classSerializeMap.end()) {
            SerializeValue(node, AttributeClassType, it->second);
        } else {
            SerializeValue(node, AttributeClassType, "other");
        }
    }

    void  Group::DeserializeClass(JSONNode *node) {
        std::string classString;
        DeserializeValue(node, AttributeClassType, &classString, "other");

        _classType = GROUPCLASS_OTHER;

        for (auto it = _classSerializeMap.begin(); it != _classSerializeMap.end(); ++it) {
            if (it->second == classString) {
                _classType = it->first;
                break;
            }
        }
    }

}  // namespace huestream
