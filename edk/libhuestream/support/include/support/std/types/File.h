/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <stddef.h>
#include <string>

namespace support {

    struct File {
        /** file data */
        std::string data;
        /** bytes still need to be sent */
        size_t size = 0;
    };
    
}  // namespace support

