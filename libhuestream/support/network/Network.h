/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_NETWORK_H_
#define NETWORK_NETWORK_H_

#include <string>
#include <vector>

#include "network/NetworkInterface.h"
#include "std/types/Vector.h"

namespace huesdk_lib {

    class Network {
    public:
        /**
         Get all available network interfaces
         @return List of the found network interfaces
         */
        static const std::vector<NetworkInterface> get_network_interfaces();
        static void set_default_network_interface(std::string ip, std::string name, NetworkInetType type);

    private:
        /**
         Constructor declared private, so no instance of this class can be created
         */
        Network();
        static bool _default_network_interface_set;
        static NetworkInterface _default_network_interface;
    };

}  // namespace huesdk_lib

#endif  // NETWORK_NETWORK_H_
