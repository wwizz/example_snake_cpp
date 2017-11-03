/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_STREAM_ISTREAMSTARTER_H_
#define HUESTREAM_STREAM_ISTREAMSTARTER_H_

#include <memory>

namespace huestream {

    class IStreamStarter {
    public:
        virtual ~IStreamStarter() {}

        virtual bool Start(bool force) = 0;

        virtual void Stop() = 0;
    };

    typedef std::shared_ptr<IStreamStarter> StreamStarterPtr;
}  // namespace huestream

#endif  // HUESTREAM_STREAM_ISTREAMSTARTER_H_
