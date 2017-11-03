/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <regex>
#include <string>

#include "network/NetworkInterface.h"

using std::regex;
using std::regex_search;
using std::smatch;
using std::string;

namespace huesdk_lib {

    NetworkInterface::NetworkInterface() {
        _inet_type = NetworkInetType ::INET_IPV4;
        _up = false;
        _loopback = false;
    }

    NetworkInterface::NetworkInterface(const string& ip, NetworkInetType inet_type, const string& name, bool up, bool loopback) :
                                                                                                                _name(string(name)),
                                                                                                                _ip(string(ip)),
                                                                                                                _inet_type(inet_type),
                                                                                                                _up(up),
                                                                                                                _loopback(loopback) { }

    const string& NetworkInterface::get_name() const {
        return _name;
    }

    void NetworkInterface::set_name(const string& name) {
        _name = string(name);
    }

    const string& NetworkInterface::get_ip() const {
        return _ip;
    }
    
    void NetworkInterface::set_ip(const string& ip) {
        _ip = string(ip);
    }
    
    NetworkInetType NetworkInterface::get_inet_type() const {
        return _inet_type;
    }
    
    void NetworkInterface::set_inet_type(NetworkInetType inet_type) {
        _inet_type = inet_type;
    }

    bool NetworkInterface::is_up() const {
        return _up;
    }

    void NetworkInterface::set_up(bool up) {
        _up = up;
    }

    bool NetworkInterface::is_loopback() const {
        return _loopback;
    }

    void NetworkInterface::set_loopback(bool loopback) {
        _loopback = loopback;
    }
    
    bool NetworkInterface::is_private() const {
        if (_inet_type != INET_IPV4) {
            // Only IPV4 supported for now
            return false;
        }
        
        smatch result;
        // Split the ip into pieces
        if (!regex_search(_ip, result, regex("^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})$"))) {
            return false;
        }

        // Get all bytes from the ip address
        int b1 = atoi(string(result[1].first, result[1].second).c_str());
        int b2 = atoi(string(result[2].first, result[2].second).c_str());
        //  unsigned char b3 = atoi(string(result[3].first, result[3].second).c_str())
        //  unsigned char b4 = atoi(string(result[4].first, result[4].second).c_str())

        // 10.x.x.x
        // 127.0.0.1
        if (b1 == 10
            || b1 == 127) {
            return true;
        }
        // 172.16.0.0 - 172.31.255.255
        if ((b1 == 172)
            && (b2 >= 16)
            && (b2 <= 31)) {
            return true;
        }
        // 192.168.0.0 - 192.168.255.255
        if ((b1 == 192) && (b2 == 168))
            return true;

        return false;
    }

}  // namespace huesdk_lib
