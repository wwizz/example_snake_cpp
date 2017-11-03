/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_COMMON_TIME_ITIMEMANAGER_H_
#define HUESTREAM_COMMON_TIME_ITIMEMANAGER_H_

#include <huestream/common/time/ITimeProvider.h>

#include <memory>

namespace huestream {

    class ITimeManager : public ITimeProvider {
    public:
        virtual void Sleep(int64_t duration) = 0;

        virtual void UpdateTime() = 0;
    };

    typedef std::shared_ptr<ITimeManager> TimeManagerPtr;
}  // namespace huestream

#endif  // HUESTREAM_COMMON_TIME_ITIMEMANAGER_H_
