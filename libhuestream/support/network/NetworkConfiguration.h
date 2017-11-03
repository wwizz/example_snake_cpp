/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_NETWORKCONFIGURATION_H_
#define NETWORK_NETWORKCONFIGURATION_H_

#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <utility>

#include "network/NetworkInterface.h"
#include "std/types/Vector.h"

namespace huesdk_lib {

    class NetworkConfiguration {
    public:
        static bool is_ssl_check_disabled();

        static void set_ssl_check_disabled(bool disabled);

        static bool get_reuse_connections();

        static void set_reuse_connections(bool reuse);

        static void unpin_root_certificate(const std::string& domain);
        
        // domain name (e.g. "api.meethue.com"), certificate in pem format
        static void pin_root_certificate(const std::string& domain, const std::string& certificate);

        // returns empty string if the domain specified in the url is not pinned
        static std::shared_ptr<std::string> get_pinned_certificate(const std::string& url);

    private:
        class CertificateStore {
        public:
            CertificateStore();
            void add_certificate(const std::string& domain_regex, const std::string& certificate);
            void remove_certificate(const std::string& domain_regex);
            std::shared_ptr<std::string> get_certificate(const std::string& domain_name) const;
        private:
            std::map<std::string, std::pair<std::regex, std::shared_ptr<std::string>>> _store;
        };

        static std::mutex _mutex;
        static bool _disable_ssl_check;
        static bool _reuse_connections;
        static CertificateStore _certificate_store;
    };

}  // namespace huesdk_lib

#endif  // NETWORK_NETWORKCONFIGURATION_H_
