/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifdef SDK_TEST

#ifndef NETWORK__TEST_NETWORK_H_
#define NETWORK__TEST_NETWORK_H_

#include <string>
#include <vector>

#include "network/NetworkInterface.h"
#include "std/types/Vector.h"

namespace huesdk_lib {

    class Network {
    public:
        /**
         @see lib/network/Network.h
         */
        static const std::vector<NetworkInterface> get_network_interfaces();

        /**
         @see lib/network/Network.h
         */
        static void set_default_network_interface(std::string ip, std::string name, NetworkInetType type);

    private:
        /**
         @see lib/network/Network.h
         */
        Network();

        static bool _default_network_interface_set;
        static NetworkInterface _default_network_interface;
    };

}  // namespace huesdk_lib

#endif  // NETWORK__TEST_NETWORK_H_

#endif  // SDK_TEST  // NOLINT
