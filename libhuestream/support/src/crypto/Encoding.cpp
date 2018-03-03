/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <mbedtls/base64.h>

#include <cassert>
#include <string>
#include <sstream>

#include "support/crypto/Encoding.h"

namespace support {

    std::string Encoding::base64_encode(const std::string& data) {
        size_t dlen;
        size_t slen = data.size();

        // get the size of the encoded string
        mbedtls_base64_encode(nullptr, 0, &dlen, nullptr, slen);

        unsigned char* dst = new unsigned char[dlen];
        const unsigned char* src = reinterpret_cast<const unsigned char*>(data.c_str());

        mbedtls_base64_encode(dst, dlen, &dlen, src, slen);

        std::string encoded = std::string(reinterpret_cast<const char*>(dst), dlen) + "\n";
        delete [] dst;
        return encoded;
    }

    std::string Encoding::hex_encode(const std::string& data) {
        size_t data_size = data.length();
        std::ostringstream hex;
        
        for (size_t i = 0; i < data_size; i++) {
            char j = (data[i] >> 4) & 0xf;
            if (j <= 9)
                hex << char(j + '0');
            else
                hex << char(j + 'a' - 10);
            j = data[i] & 0xf;
            if (j <= 9)
                hex << char(j + '0');
            else
                hex << char(j + 'a' - 10);
        }
        return hex.str();
    }

}  // namespace support
