/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef STD_TYPES_FILE_H_
#define STD_TYPES_FILE_H_

#include <stddef.h>
#include <string>

namespace huesdk_lib {

    struct File {
        /** file data */
        std::string data;
        /** bytes still need to be sent */
        size_t size = 0;
    };
    
}  // namespace huesdk_lib

#endif  // STD_TYPES_FILE_H_

