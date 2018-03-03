/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_STREAM_STREAMSTARTER_H_
#define HUESTREAM_STREAM_STREAMSTARTER_H_

#include <huestream/common/data/Bridge.h>
#include <huestream/stream/IStreamStarter.h>
#include <huestream/common/http/IHttpClient.h>

namespace huestream {

    class StreamStarter : public IStreamStarter {
    public:
        StreamStarter(BridgePtr bridge, HttpClientPtr _http);
        ~StreamStarter();

        bool StartStream(ActivationOverrideLevel overrideLevel) override;
        bool Start(bool force) override;
        void Stop() override;
        bool DeactivateGroup(std::string groupId) override;


    protected:
        bool Execute(bool activate);
        bool Execute(std::string url, bool activate);

        bool CheckForErrors(HttpRequestPtr req);

        static bool IsNotExistingOrInvalidGroup(int errorCode);

        BridgePtr _bridge;
        HttpClientPtr _http;
    };

}  // namespace huestream

#endif  // HUESTREAM_STREAM_STREAMSTARTER_H_
