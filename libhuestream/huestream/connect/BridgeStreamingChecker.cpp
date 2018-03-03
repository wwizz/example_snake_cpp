/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#include <huestream/connect/BridgeStreamingChecker.h>
#include <memory>

namespace huestream {

BridgeStreamingChecker::BridgeStreamingChecker(FullConfigRetrieverPtr fullConfigRetrieverPtr)
    : _fullConfigRetrieverPtr(fullConfigRetrieverPtr) {}


void BridgeStreamingChecker::Check(BridgePtr bridge) {
    if (!bridge->IsConnectable()) {
        return;
    }

    _fullConfigRetrieverPtr->Execute(bridge->Clone(), [=](OperationResult result, BridgePtr actualBridge) {
        if (!bridge->IsConnected() && !actualBridge->IsConnected())
            return;

        auto differences = CompareBridges(bridge, actualBridge);
        for (auto const& feedbackId : differences) {
            _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, feedbackId, actualBridge));
        }
    });
}

void BridgeStreamingChecker::SetFeedbackMessageCallback(std::function<void(const huestream::FeedbackMessage &)> callback) {
    _messageCallback = callback;
}

static bool IsBridgeChanged(BridgePtr oldBridge, BridgePtr newBridge);
static bool AreGroupsUpdated(GroupListPtr expectedGroups, GroupListPtr actualGroups);
static bool AreLightsUpdated(LightListPtr expectedLights, LightListPtr actualLights);
static bool IsLightUpdated(LightPtr expectedLight, LightPtr actualLight);
static bool IsPositionLightUpdated(const Location &expectedLightPosition, const Location &actualLightPosition);
static bool IsHomeAutomationStateUpdated(GroupPtr expectedGroup, GroupPtr actualGroup);

std::vector<FeedbackMessage::Id> CompareBridges(BridgePtr oldBridge, BridgePtr newBridge) {
    std::vector<FeedbackMessage::Id> differences;

    if (!oldBridge->IsConnected() && newBridge->IsConnected()) {
        differences.push_back(FeedbackMessage::ID_BRIDGE_CONNECTED);
    }
    if (!oldBridge->IsStreaming() && newBridge->IsStreaming()) {
        differences.push_back(FeedbackMessage::ID_STREAMING_CONNECTED);
    }
    if (oldBridge->IsStreaming() && !newBridge->IsStreaming()) {
        differences.push_back(FeedbackMessage::ID_STREAMING_DISCONNECTED);
    }
    if (oldBridge->IsConnected() && !newBridge->IsConnected()) {
        differences.push_back(FeedbackMessage::ID_BRIDGE_DISCONNECTED);
    }

    if (IsBridgeChanged(oldBridge, newBridge)) {
        differences.push_back(FeedbackMessage::ID_BRIDGE_CHANGED);
    }

    if (AreGroupsUpdated(oldBridge->GetGroups(), newBridge->GetGroups())) {
        differences.push_back(FeedbackMessage::ID_GROUPLIST_UPDATED);
    }

    if (AreLightsUpdated(oldBridge->GetGroupLights(), newBridge->GetGroupLights())) {
        differences.push_back(FeedbackMessage::ID_LIGHTS_UPDATED);
    }

    if (!newBridge->IsStreaming() && IsHomeAutomationStateUpdated(oldBridge->GetGroup(), newBridge->GetGroup())) {
        differences.push_back(FeedbackMessage::ID_GROUP_LIGHTSTATE_UPDATED);
    }

    return differences;
}

static bool IsBridgeChanged(BridgePtr oldBridge, BridgePtr newBridge) {
    return (oldBridge->GetId() != newBridge->GetId() ||
        oldBridge->GetName() != newBridge->GetName() ||
        oldBridge->GetIpAddress() != newBridge->GetIpAddress() ||
        oldBridge->GetModelId() != newBridge->GetModelId() ||
        oldBridge->GetApiversion() != newBridge->GetApiversion() ||
        oldBridge->GetUser() != newBridge->GetUser() ||
        oldBridge->GetClientKey() != newBridge->GetClientKey());
}

static bool AreGroupsUpdated(GroupListPtr expectedGroups, GroupListPtr actualGroups) {
    if (actualGroups->size() != expectedGroups->size()) {
        return true;
    }

    for (size_t i = 0; i < actualGroups->size(); ++i) {
        if (expectedGroups->at(i)->GetId() != actualGroups->at(i)->GetId() ||
            expectedGroups->at(i)->GetName() != actualGroups->at(i)->GetName() ||
            expectedGroups->at(i)->Active() != actualGroups->at(i)->Active() ||
            expectedGroups->at(i)->GetOwner() != actualGroups->at(i)->GetOwner()) {
            return true;
        }
    }
    return false;
}

static bool AreLightsUpdated(LightListPtr expectedLights, LightListPtr actualLights) {
    if (actualLights->size() != expectedLights->size()) {
        return true;
    }

    for (size_t i = 0; i < actualLights->size(); ++i) {
        if (IsLightUpdated(expectedLights->at(i), actualLights->at(i))) {
            return true;
        }
    }
    return false;
}

static bool IsLightUpdated(LightPtr expectedLight, LightPtr actualLight) {
    return expectedLight->GetId() != actualLight->GetId() ||
        IsPositionLightUpdated(expectedLight->GetPosition(), actualLight->GetPosition());
}

static bool IsPositionLightUpdated(const Location &expectedLightPosition,
    const Location &actualLightPosition) {
    return expectedLightPosition.GetX() != actualLightPosition.GetX() ||
        expectedLightPosition.GetY() != actualLightPosition.GetY();
}

static bool IsHomeAutomationStateUpdated(GroupPtr expectedGroup, GroupPtr actualGroup) {
    if (actualGroup == nullptr) {
        return false;
    }
    if (expectedGroup == nullptr) {
        return true;
    }
    return expectedGroup->OnState() != actualGroup->OnState() ||
        expectedGroup->GetBrightnessState() != actualGroup->GetBrightnessState();
}

}  // namespace huestream
