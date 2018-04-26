/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "support/network/NetworkInterface.h"
#include "support/std/types/VectorOperations.h"

namespace support {

    class NetworkConfiguration {
    public:
        static bool is_ssl_check_disabled();

        static void set_ssl_check_disabled(bool disabled);

        static bool get_reuse_connections();

        static void set_reuse_connections(bool reuse);

        // returns a default list if the domain specified in the url is not "pinned"
        static std::vector<std::string> get_trusted_certificates(const std::string& url);
        
    private:
        static std::mutex _mutex;
        static bool _disable_ssl_check;
        static bool _reuse_connections;
    };

}  // namespace support

