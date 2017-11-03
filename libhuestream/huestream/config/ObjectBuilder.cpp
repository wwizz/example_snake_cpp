/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/effect/animation/animations/ConstantAnimation.h>
#include <huestream/effect/animation/animations/CurveAnimation.h>
#include <huestream/effect/animation/animations/RandomAnimation.h>
#include <huestream/effect/animation/animations/SequenceAnimation.h>
#include <huestream/effect/animation/animations/TweenAnimation.h>
#include <huestream/common/data/Scene.h>
#include <huestream/common/data/Light.h>
#include <huestream/common/data/Group.h>
#include <huestream/common/data/Bridge.h>
#include <huestream/effect/effects/AreaEffect.h>
#include <huestream/effect/effects/LightSourceEffect.h>
#include <huestream/effect/lightscript/Action.h>
#include <huestream/effect/effects/MultiChannelEffect.h>
#include <huestream/config/ObjectBuilder.h>
#include <huestream/effect/lightscript/LightScript.h>
#include <huestream/effect/effects/base/RadialEffect.h>
#include <huestream/effect/effects/LightIteratorEffect.h>

#include <memory>
#include <string>

namespace huestream {

ObjectBuilder::ObjectBuilder(std::shared_ptr<BridgeSettings> bridgeSettings) :
    _bridgeSettings(bridgeSettings) {
}

std::shared_ptr<Serializable> ObjectBuilder::ConstructInstanceOf(std::string type) {
    if (type == LightScript::type) return std::make_shared<LightScript>();
    if (type == ConstantAnimation::type) return std::make_shared<ConstantAnimation>();
    if (type == CurveAnimation::type) return std::make_shared<CurveAnimation>();
    if (type == RandomAnimation::type) return std::make_shared<RandomAnimation>();
    if (type == SequenceAnimation::type) return std::make_shared<SequenceAnimation>();
    if (type == TweenAnimation::type) return std::make_shared<TweenAnimation>();
    if (type == Light::type) return std::make_shared<Light>();
    if (type == Color::type) return std::make_shared<Color>();
    if (type == Location::type) return std::make_shared<Location>();
    if (type == Bridge::type) return std::make_shared<Bridge>(_bridgeSettings);
    if (type == Group::type) return std::make_shared<Group>();
    if (type == Area::type) return std::make_shared<Area>();
    if (type == Channel::type) return std::make_shared<Channel>();
    if (type == AreaEffect::type) return std::make_shared<AreaEffect>();
    if (type == LightSourceEffect::type) return std::make_shared<LightSourceEffect>();
    if (type == LightIteratorEffect::type) return std::make_shared<LightIteratorEffect>();
    if (type == MultiChannelEffect::type) return std::make_shared<MultiChannelEffect>();
    if (type == Action::type) return std::make_shared<Action>();
    if (type == Scene::type) return std::make_shared<Scene>();

    return nullptr;
}

}  // namespace huestream
