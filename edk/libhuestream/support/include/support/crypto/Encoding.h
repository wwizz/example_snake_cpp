/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>

namespace support {

    class Encoding {
    public:
        static std::string base64_encode(const std::string& data);
        
        static std::string hex_encode(const std::string& data);
    };
    
}  // namespace support

