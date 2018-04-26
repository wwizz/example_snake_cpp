/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
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
};

std::vector<FeedbackMessage::Id> CompareBridges(BridgePtr oldBridge, BridgePtr newBridge);

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_BRIDGESTREAMINGCHECKER_H_
