/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/data/Color.h>

#include <algorithm>
#include <string>
#include <vector>

namespace huestream {

PROP_IMPL(Color, double, red, R);
PROP_IMPL(Color, double, green, G);
PROP_IMPL(Color, double, blue, B);
PROP_IMPL(Color, double, alpha, Alpha);


Color::Color() : _red(0), _green(0), _blue(0), _alpha(0) {}

Color::Color(double red, double green, double blue)
    : _red(red), _green(green), _blue(blue), _alpha(1) {}

Color::Color(double red, double green, double blue, double alpha)
    : _red(red), _green(green), _blue(blue), _alpha(alpha) {}


std::vector<double> Color::GetRGBA() const {
    auto v = std::vector<double>();
    v.push_back(_red);
    v.push_back(_green);
    v.push_back(_blue);
    v.push_back(_alpha);

    return v;
}

double Color::GetCurrentBrightness() const {
    double max = std::max(std::max(_red, _green), _blue);
    return max;
}

void Color::ApplyBrightness(double value) {
    value = Clamp(value);

    double currentBri = GetCurrentBrightness();

    // set to max
    _red /= currentBri;
    _green /= currentBri;
    _blue /= currentBri;

    _red *= value;
    _green *= value;
    _blue *= value;
}

void Color::Clamp() {
    _red = Clamp(_red);
    _green = Clamp(_green);
    _blue = Clamp(_blue);
    _alpha = Clamp(_alpha);
}

void Color::Serialize(JSONNode *node) const {
    Serializable::Serialize(node);
    SerializeValue(node, AttributeR, _red);
    SerializeValue(node, AttributeG, _green);
    SerializeValue(node, AttributeB, _blue);
    SerializeValue(node, AttributeAlpha, _alpha);
}

void Color::Deserialize(JSONNode *node) {
    Serializable::Deserialize(node);
    DeserializeValue(node, AttributeR, &_red, 0);
    DeserializeValue(node, AttributeG, &_green, 0);
    DeserializeValue(node, AttributeB, &_blue, 0);
    DeserializeValue(node, AttributeAlpha, &_alpha, 0);
}

std::string Color::GetTypeName() const {
    return type;
}

double Color::Clamp(double value) {
    if (value > 1) return 1;
    if (value < 0) return 0;
    return value;
}

}  // namespace huestream
