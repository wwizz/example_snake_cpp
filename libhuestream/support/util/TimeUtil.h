/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef UTIL_TIMEUTIL_H_
#define UTIL_TIMEUTIL_H_

#include <time.h>
#include <mutex>

using std::mutex;
using std::tm;

 #ifndef _WIN32
    using std::time_t;
#endif

namespace huesdk_lib {
    
    /** ensure thread safety of the time functions */
    extern mutex time_mutex;
    
    /**
     The inverse of localtime and gmtime
     @return Seconds since epoch in UTC
     */
    extern time_t timegm(tm* time_struct);

    /**
     Thread safe extension of mktime
     @link http://www.cplusplus.com/reference/ctime/mktime/
     */
    extern time_t mktime(tm* time_struct);
    
    /**
     Thread safe extension of gmtime
     @link http://www.cplusplus.com/reference/ctime/gmtime/
     */
    extern tm gmtime(const time_t* time);  // NOLINT

    /**
     Thread safe extension of localtime
     @link http://www.cplusplus.com/reference/ctime/localtime/
     */
    extern tm localtime(const time_t* time);  // NOLINT
    
}  // namespace huesdk_lib

#endif  // UTIL_TIMEUTIL_H_
