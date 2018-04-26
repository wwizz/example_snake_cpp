/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_STREAM_STREAMFACTORY_H_
#define HUESTREAM_STREAM_STREAMFACTORY_H_

#include <huestream/stream/IStreamFactory.h>
#include <huestream/common/http/IHttpClient.h>

namespace huestream {

    class StreamFactory : public IStreamFactory {
    public:
        StreamFactory();

        StreamStarterPtr CreateStreamStarter(BridgePtr bridge) override;
    };

}  // namespace huestream

#endif  // HUESTREAM_STREAM_STREAMFACTORY_H_
