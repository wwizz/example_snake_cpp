/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/http/HttpClient.h>
#include <huestream/stream/StreamFactory.h>
#include <huestream/stream/StreamStarter.h>

#include <memory>

namespace huestream {

    StreamFactory::StreamFactory() {}

    StreamStarterPtr StreamFactory::CreateStreamStarter(BridgePtr bridge) {
        return std::make_shared<StreamStarter>(bridge, std::make_shared<HttpClient>());
    }

}  // namespace huestream
