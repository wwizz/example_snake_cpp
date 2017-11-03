/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <map>
#include <memory>
#include <string>
#include <regex>
#include <utility>

#include "network/NetworkConfiguration.h"

#define Q(x) #x
#define QUOTE(x) Q(x)

namespace huesdk_lib {

    std::mutex NetworkConfiguration::_mutex;

    bool NetworkConfiguration::_disable_ssl_check = false;

    bool NetworkConfiguration::_reuse_connections = true;

    NetworkConfiguration::CertificateStore NetworkConfiguration::_certificate_store;

    NetworkConfiguration::CertificateStore::CertificateStore() {
        std::map<std::string, std::string> mapping = {
#ifdef CERTIFICATE_MAP_FILE
#include QUOTE(CERTIFICATE_MAP_FILE)
#endif
        };

        for (auto& item : mapping) {
            add_certificate(item.first, item.second);
        }
    }

    void NetworkConfiguration::CertificateStore::add_certificate(const std::string& domain_regex, const std::string& certificate) {
#ifdef HTTP_CLIENT_OBJC
        std::string certificate_mutable = certificate;
        certificate_mutable.erase(std::remove(certificate_mutable.begin(), certificate_mutable.end(), '\n'), certificate_mutable.end());
        static std::regex certificate_regex(".*?-----BEGIN CERTIFICATE-----?(.*)-----END CERTIFICATE-----.*?");
        
        std::cmatch what;
        std::string certificate_contnet;
        if (std::regex_match(certificate_mutable.c_str(), what, certificate_regex)) {
            certificate_contnet = std::string(what[1].first, what[1].second);
        }
        
        _store[domain_regex] = std::make_pair(std::regex(domain_regex), std::make_shared<std::string>(certificate_contnet));
#else
        _store[domain_regex] = std::make_pair(std::regex(domain_regex), std::make_shared<std::string>(certificate));
#endif
    }

    void NetworkConfiguration::CertificateStore::remove_certificate(const std::string& domain_regex) {
        _store.erase(domain_regex);
    }

    std::shared_ptr<std::string> NetworkConfiguration::CertificateStore::get_certificate(const std::string& domain_name) const {
        for (auto& item : _store) {
            if (std::regex_match(domain_name.c_str(), item.second.first)) {
                return item.second.second;
            }
        }
        return {};
    }

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

    void NetworkConfiguration::unpin_root_certificate(const std::string& domain) {
        std::lock_guard<std::mutex> lk(_mutex);
        _certificate_store.remove_certificate(domain);
    }
    
    void NetworkConfiguration::pin_root_certificate(const std::string& domain, const std::string& certificate) {
        std::lock_guard<std::mutex> lk(_mutex);
        _certificate_store.add_certificate(domain, certificate);
    }

    std::shared_ptr<std::string> NetworkConfiguration::get_pinned_certificate(const std::string& url) {
        static std::regex url_regex("(http|https)://([^/ :]+)(:\\d+)?.*");
        
        std::cmatch what;
        std::string domain;
        if (std::regex_match(url.c_str(), what, url_regex)) {
            domain = std::string(what[2].first, what[2].second);
        }
        
        std::lock_guard<std::mutex> lk(_mutex);
        return _certificate_store.get_certificate(domain);
    }

}  // namespace huesdk_lib
