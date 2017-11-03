/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/Location.h>

#include <string>

namespace huestream {

PROP_IMPL(Location, double, x, X);
PROP_IMPL(Location, double, y, Y);

Location::Location(double x, double y)
    : _x(x), _y(y) {
}

Location::Location()
    : _x(0), _y(0) {
}

Location::~Location() {
}

std::string Location::GetTypeName() const {
    return type;
}

void Location::Serialize(JSONNode *node) const {
    Serializable::Serialize(node);
    SerializeValue(node, AttributeX, _x);
    SerializeValue(node, AttributeY, _y);
}

void Location::Deserialize(JSONNode *node) {
    Serializable::Deserialize(node);
    DeserializeValue(node, AttributeX, &_x, 0);
    DeserializeValue(node, AttributeY, &_y, 0);
}

}  // namespace huestream
