/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/

#ifndef HUESTREAM_CONNECT_BRIDGESTREAMINGCHECKER_H_
#define HUESTREAM_CONNECT_BRIDGESTREAMINGCHECKER_H_

#include <huestream/connect/IBridgeStreamingChecker.h>
#include <huestream/connect/IFullConfigRetriever.h>
#include <huestream/connect/FeedbackMessage.h>

namespace huestream {

class BridgeStreamingChecker: public IBridgeStreamingChecker {
public:
    explicit BridgeStreamingChecker(FullConfigRetrieverPtr _fullConfigRetrieverPtr);
    void SetFeedbackMessageCallback(std::function<void(const huestream::FeedbackMessage &)> callback) override;
    void Check(BridgePtr bridge) override;

private:
    FullConfigRetrieverPtr _fullConfigRetrieverPtr;

    std::function<void(const huestream::FeedbackMessage &)> _messageCallback;
    bool IsPositionLightUpdated(const Location &expectedLightPosition, const Location &actualLightPosition);
    bool IsLightUpdated(LightPtr expectedLight, LightPtr actualLight);
    bool AreLightsUpdated(LightListPtr expectedLights, LightListPtr actualLights);
    bool AreGroupsUpdated(GroupListPtr expectedGroups, GroupListPtr actualGroups);
    bool IsHomeAutomationStateUpdated(GroupPtr expectedGroup, GroupPtr actualGroup);
};

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_BRIDGESTREAMINGCHECKER_H_