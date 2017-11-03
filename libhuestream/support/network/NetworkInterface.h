/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_NETWORKINTERFACE_H_
#define NETWORK_NETWORKINTERFACE_H_

#include <string>

#include "util/StringUtil.h"

using std::string;

namespace huesdk_lib {

    enum NetworkInetType {
        INET_IPV4,
        INET_IPV6
    };

    class NetworkInterface {
    public:
        /**
         Construct without data
         */
        NetworkInterface();

        /**
         Construct with data
         @param ip        The ip address
                          e.g. 192.168.1.1 (IPV4),
                               2607:f0d0:1002:51::4 (IPV6)
         @param inet_type The type of the network interface: IPV4 or IPV6
         @param name      The name of the network interface, e.g. "eth0"
         @param up        Whether the network interface is enabled
         @param loopback  Whether the network interface is a loopback
         */
        NetworkInterface(const string& ip, NetworkInetType inet_type, const string& name, bool up, bool loopback);
        
        /**
         Get name
         @return The name of the network interface
         */
        const string& get_name() const;
        
        /**
         Set name
         @param name The name of the network interface
         */
        void set_name(const string& name);

        /**
         Get ip address
         @return The ip address of the network interface
         */
        const string& get_ip() const;
        
        /**
         Set ip
         @param ip Set the ip address of the network interface
         */
        void set_ip(const string& ip);
        
        /**
         Get inet type
         @return The inet type of the network interface: IPV4 or IPV6
         */
        NetworkInetType get_inet_type() const;
        
        /**
         Set inet type
         @param inet_type The inet type of the network interface: IPV4 or IPV6
         */
        void set_inet_type(NetworkInetType inet_type);
        
        /**
         Whether the network interface is enabled
         @return true if enabled, false otherwise
         */
        bool is_up() const;
        
        /**
         Set whether the netwok interface is enabled
         @param up Whether the network interface is enabled
         */
        void set_up(bool up);
        
        /**
         Whether the network interface is a loopback
         @return true if loopback, false otherwise
         */
        bool is_loopback() const;
        
        /**
         Set whether the network interface is a loopback
         @param loopback Whether the network interface is a loopback
         */
        void set_loopback(bool loopback);
        
        /**
         Whether this is a private ip according to the RFC1918, only for IPV4
         @return true when the ip is private, false otherwise
         */
        bool is_private() const;
        
    private:
        /** the name, e.g. "eth0" */
        string          _name;
        /** the ip address
            format: 
            - IPV4: 192.168.1.1
            - IPV6: 2607:f0d0:1002:51::4 */
        string          _ip;
        /** the inet type: IPV4 or IPV6 */
        NetworkInetType _inet_type;
        /** whether the network interface is enabled */
        bool            _up;
        /** whether the network interface is a loopback */
        bool            _loopback;
    };

}  // namespace huesdk_lib

#endif  // NETWORK_NETWORKINTERFACE_H_
