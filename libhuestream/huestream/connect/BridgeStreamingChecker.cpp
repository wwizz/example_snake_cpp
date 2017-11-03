/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
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

    _fullConfigRetrieverPtr->Execute(std::make_shared<Bridge>(*bridge), [=](OperationResult result, BridgePtr actualBridge) {
        if (result == OPERATION_FAILED) {
            if (bridge->IsStreaming()) {
                _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_STREAMING_DISCONNECTED, actualBridge));
            }
            if (bridge->IsConnected()) {
                _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_BRIDGE_DISCONNECTED, actualBridge));
            }
            return;
        }

        if (!bridge->IsConnected() && actualBridge->IsConnected()) {
            _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_BRIDGE_CONNECTED, actualBridge));
        } else if (bridge->IsStreaming() && !actualBridge->IsStreaming()) {
            _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_STREAMING_DISCONNECTED, actualBridge));
        }

        if (AreGroupsUpdated(bridge->GetGroups(), actualBridge->GetGroups())) {
            _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_GROUPLIST_UPDATED, actualBridge));
        }

        if (bridge->IsValidGroupSelected() && actualBridge->IsValidGroupSelected()) {
            if (AreLightsUpdated(bridge->GetGroup()->GetLights(), actualBridge->GetGroup()->GetLights())) {
                _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_LIGHTS_UPDATED, actualBridge));
            }
            if (!actualBridge->IsStreaming() && IsHomeAutomationStateUpdated(bridge->GetGroup(), actualBridge->GetGroup())) {
                _messageCallback(FeedbackMessage(FeedbackMessage::REQUEST_TYPE_INTERNAL, FeedbackMessage::ID_GROUP_LIGHTSTATE_UPDATED, actualBridge));
            }
        }
    });
}

bool BridgeStreamingChecker::AreLightsUpdated(LightListPtr expectedLights, LightListPtr actualLights) {
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

bool BridgeStreamingChecker::IsLightUpdated(LightPtr expectedLight, LightPtr actualLight) {
    return expectedLight->GetId() != actualLight->GetId() ||
           IsPositionLightUpdated(expectedLight->GetPosition(), actualLight->GetPosition());
}

bool BridgeStreamingChecker::IsPositionLightUpdated(const Location &expectedLightPosition,
                                                const Location &actualLightPosition) {
    return expectedLightPosition.GetX() != actualLightPosition.GetX() ||
           expectedLightPosition.GetY() != actualLightPosition.GetY();
}

bool BridgeStreamingChecker::AreGroupsUpdated(GroupListPtr expectedGroups, GroupListPtr actualGroups) {
    if (actualGroups->size() != expectedGroups->size()) {
        return true;
    }

    for (size_t i = 0; i < actualGroups->size(); ++i) {
        if (expectedGroups->at(i)->GetId() != actualGroups->at(i)->GetId() || expectedGroups->at(i)->Active() != actualGroups->at(i)->Active()) {
            return true;
        }
    }
    return false;
}

bool BridgeStreamingChecker::IsHomeAutomationStateUpdated(GroupPtr expectedGroup, GroupPtr actualGroup) {
    return expectedGroup->OnState() != actualGroup->OnState() ||
        expectedGroup->GetBrightnessState() != actualGroup->GetBrightnessState();
}

void BridgeStreamingChecker::SetFeedbackMessageCallback(std::function<void(const huestream::FeedbackMessage &)> callback) {
    _messageCallback = callback;
}
}  // namespace huestream