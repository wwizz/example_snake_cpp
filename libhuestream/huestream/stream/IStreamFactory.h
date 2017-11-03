/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_STREAM_ISTREAMFACTORY_H_
#define HUESTREAM_STREAM_ISTREAMFACTORY_H_

#include <huestream/stream/IStreamStarter.h>
#include <huestream/common/data/Bridge.h>
#include <huestream/common/http/IHttpClient.h>

#include <memory>

namespace huestream {

    class IStreamFactory {
    public:
        virtual ~IStreamFactory() {
        }

        virtual StreamStarterPtr CreateStreamStarter(BridgePtr bridge) = 0;
    };

    typedef std::shared_ptr<IStreamFactory> StreamFactoryPtr;

}  // namespace huestream

#endif  // HUESTREAM_STREAM_ISTREAMFACTORY_H_
