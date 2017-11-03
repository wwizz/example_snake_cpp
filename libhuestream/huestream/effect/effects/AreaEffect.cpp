/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/effect/animation/animations/ConstantAnimation.h>
#include <huestream/effect/effects/AreaEffect.h>

#include <string>
#include <memory>

namespace huestream {

    PROP_IMPL(AreaEffect, AreaListPtr, areas, Areas);

    AreaEffect::AreaEffect(std::string name, unsigned int layer, bool isAlphaBoundToBrightness) :
            ColorAnimationEffect(name, layer, isAlphaBoundToBrightness),
            _areas(std::make_shared<AreaList>()) {
    }

    AreaEffect::~AreaEffect() {
    }

    Color AreaEffect::GetColor(LightPtr light) {
        for (auto area : *_areas) {
            if (area->isInArea(light->GetPosition())) {
                auto color = Color(_r->GetValue(), _g->GetValue(), _b->GetValue(), _a->GetValue());
                SetIntensity(&color);
                return color;
            }
        }
        return Color();
    }


    void AreaEffect::RenderUpdate() {
    }

    void AreaEffect::AddArea(const Area &area) {
        _areas->push_back(std::make_shared<Area>(area));
    }

    void AreaEffect::SetArea(const Area &area) {
        _areas->clear();
        AddArea(area);
    }
    std::string AreaEffect::GetTypeName() const {
        return type;
    }

    void AreaEffect::Serialize(JSONNode *node) const {
        ColorAnimationEffect::Serialize(node);
        SerializeList(node, AttributeAreas, _areas);
    }

    void AreaEffect::Deserialize(JSONNode *node) {
        ColorAnimationEffect::Deserialize(node);
        DeserializeList<AreaListPtr, Area>(node, &_areas, AttributeAreas);
    }


}  // namespace huestream
