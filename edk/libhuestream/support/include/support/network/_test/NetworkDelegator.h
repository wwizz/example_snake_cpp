/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <vector>

#include "support/network/_test/Network.h"
#include "support/network/NetworkInterface.h"

using std::shared_ptr;

namespace support {

    class NetworkDelegate {
    public:
        /**
         @see lib/network/Network.h
         */
        virtual const std::vector<NetworkInterface> get_network_interfaces() = 0;
    };
    
    // Default
    class NetworkDelegateImpl : public NetworkDelegate {
    public:
        /**
         @see lib/network/Network.h
         */
        const std::vector<NetworkInterface> get_network_interfaces() {
            // Get network interfaces from real network class
            return Network::get_network_interfaces();
        }
    };
    
    class NetworkDelegator {
    public:
        /**
         @see lib/network/Network.h
         */
        static const std::vector<NetworkInterface> get_network_interfaces();
        
        
        /* delegate */
       
        /**
         Set the delegate
         @note   Initially NetworkDelegateImpl is set as delegate
         @return The delegate, nullptr if no delegate has been set
         */
        static void set_delegate(shared_ptr<NetworkDelegate> delegate);
        
    private:
        /**
         Constructor declared private, so no instance of this class can be created
         */
        NetworkDelegator();
    };

}  // namespace support
