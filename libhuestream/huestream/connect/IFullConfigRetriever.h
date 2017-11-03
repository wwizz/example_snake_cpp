/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_IFULLCONFIGRETRIEVER_H_
#define HUESTREAM_CONNECT_IFULLCONFIGRETRIEVER_H_

#include <huestream/common/data/Bridge.h>
#include <huestream/connect/OperationResult.h>

#include <functional>
#include <memory>

namespace huestream {

    typedef std::function<void(OperationResult, BridgePtr)> RetrieveCallbackHandler;

    class IFullConfigRetriever {
    public:
        virtual ~IFullConfigRetriever() = default;

        virtual bool Execute(BridgePtr bridge, RetrieveCallbackHandler cb) = 0;
    };

    typedef std::shared_ptr<IFullConfigRetriever> FullConfigRetrieverPtr;
}  // namespace huestream

#endif  // HUESTREAM_CONNECT_IFULLCONFIGRETRIEVER_H_
