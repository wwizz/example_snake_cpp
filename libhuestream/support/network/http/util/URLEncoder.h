/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_UTIL_URLENCODER_H_
#define NETWORK_HTTP_UTIL_URLENCODER_H_

#include <string>

namespace huesdk_lib {

    /**
     URL Encoder class - for internal use only
     */
    class URLEncoder {
    public:
        /**
         url encode function - for internal use only
         @param the std::string value to encode
         @return the urlencoded value as std::string
         */
        static std::string url_encode(std::string value);
    };
    
}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_UTIL_URLENCODER_H_
