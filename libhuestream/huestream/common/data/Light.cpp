/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/Light.h>

#include <iostream>
#include <string>

namespace huestream {

Light::Light() : Light("", Location(0, 0)) {
}

Light::Light(std::string id, Location pos, std::string name, std::string model, bool reachable)
    : _id(id), _name(name), _model(model), _color(), _position(pos), _reachable(reachable) {
}

Light::~Light() {
}

PROP_IMPL(Light, std::string, id, Id);
PROP_IMPL(Light, std::string, name, Name);
PROP_IMPL(Light, std::string, model, Model);
PROP_IMPL(Light, Color, color, Color);
PROP_IMPL(Light, Location, position, Position);
PROP_IMPL_BOOL(Light, bool, reachable, Reachable);

void Light::Serialize(JSONNode *node) const {
    Serializable::Serialize(node);
    SerializeValue(node, AttributeId, _id);
    SerializeValue(node, AttributeName, _name);
    SerializeValue(node, AttributeModel, _model);
    SerializeMember(node, AttributeColor, _color);
    SerializeMember(node, AttributePosition, _position);
}

void Light::Deserialize(JSONNode *node) {
    Serializable::Deserialize(node);
    DeserializeValue(node, AttributeId, &_id, "");
    DeserializeValue(node, AttributeName, &_name, "");
    DeserializeValue(node, AttributeModel, &_model, "");
    SetMemberIfAttributeExists(node, AttributeColor, &_color);
    SetMemberIfAttributeExists(node, AttributePosition, &_position);
}

Light* Light::Clone() {
    auto l = new Light();
    l->_id = _id;
    l->_name = _name;
    l->_model = _model;
    l->_color = _color;
    l->_position = _position;
    l->_reachable = _reachable;
    return  l;
}

std::string Light::GetTypeName() const {
    return type;
}

}  // namespace huestream
