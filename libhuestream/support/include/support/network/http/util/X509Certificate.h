/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>

namespace support {

    class X509Certificate {
    public:
        static std::string der_to_pem(const unsigned char*, size_t);

        static std::string get_pem_data(const std::string& pem);
                
        static std::string extract_pem(const std::string& certificate);

        static bool are_equal(const std::string& pem1, const std::string& pem2);
    };

}  // namespace support


