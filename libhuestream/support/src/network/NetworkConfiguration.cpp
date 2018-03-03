/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <map>
#include <memory>
#include <string>
#include <regex>
#include <utility>

#include "support/network/NetworkConfiguration.h"

#define Q(x) #x
#define QUOTE(x) Q(x)

#include QUOTE(CERTIFICATE_MAP_FILE)

namespace support {
    std::mutex NetworkConfiguration::_mutex;

    bool NetworkConfiguration::_disable_ssl_check = false;

    bool NetworkConfiguration::_reuse_connections = true;

    bool NetworkConfiguration::is_ssl_check_disabled() {
        return _disable_ssl_check;
    }

    void NetworkConfiguration::set_ssl_check_disabled(bool disabled) {
        _disable_ssl_check = disabled;
    }

    bool NetworkConfiguration::get_reuse_connections() {
        return _reuse_connections;
    }

    void NetworkConfiguration::set_reuse_connections(bool reuse) {
        _reuse_connections = reuse;
    }

    std::vector<std::string> NetworkConfiguration::get_trusted_certificates(const std::string& url) {
        static std::regex url_regex("^https://([^/:]+)");
        
        std::smatch m;
        if (std::regex_search(url, m, url_regex)) {
            std::string domain_name = m[1].str();

            for (auto &item : default_certificate_mapping) {
                std::regex re = item.first;
                if (std::regex_match(domain_name, re)) {
                    return item.second;
                }
            }
        }

        return {};
    }

}  // namespace support
