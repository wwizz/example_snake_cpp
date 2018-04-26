/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_IFULLCONFIGRETRIEVER_H_
#define HUESTREAM_CONNECT_IFULLCONFIGRETRIEVER_H_

#include <huestream/common/data/Bridge.h>
#include <huestream/connect/OperationResult.h>

#include <functional>
#include <memory>

namespace huestream {
    enum class ConfigType {
        Small,
        Full
    };

    typedef std::function<void(OperationResult, BridgePtr)> RetrieveCallbackHandler;

    class IConfigRetriever {
    public:
        virtual ~IConfigRetriever() = default;

        virtual bool Execute(BridgePtr bridge, RetrieveCallbackHandler cb) = 0;
    };

    using ConfigRetrieverPtr = std::shared_ptr<IConfigRetriever>;
    using FullConfigRetrieverPtr = ConfigRetrieverPtr;
    using IFullConfigRetriever = IConfigRetriever;
}  // namespace huestream

#endif  // HUESTREAM_CONNECT_IFULLCONFIGRETRIEVER_H_
