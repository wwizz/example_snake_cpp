/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <condition_variable>
#include <mutex>
#include <string>
#include <regex>
#include <vector>

#include "libjson/libjson.h"
#include "logging/Log.h"
#include "network/http/IHttpResponse.h"
#include "network/http/HttpRequestError.h"

#ifndef SDK_TEST
#include "network/Network.h"
#include "network/http/HttpRequest.h"

using huesdk_lib::HTTP_REQUEST_STATUS_OK;

#define NETWORK      huesdk_lib::Network
#define HTTP_REQUEST huesdk_lib::HttpRequest
#else
// Test interface
#include "network/_test/NetworkDelegator.h"
#include "network/http/_test/HttpRequestDelegator.h"

using huesdk_lib::HTTP_REQUEST_STATUS_OK;

#define NETWORK      huesdk_lib::NetworkDelegator
#define HTTP_REQUEST huesdk_lib::HttpRequestDelegator
#endif

#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"
#include "bridgediscovery/BridgeInfo.h"
#include "bridgediscovery/BridgeDiscoveryConfiguration.h"

using std::condition_variable;
using std::mutex;
using std::string;
using std::smatch;
using std::regex;
using std::vector;
using huesdk_lib::IHttpResponse;
using huesdk_lib::HttpRequestError;
using huesdk_lib::INET_IPV4;
using huesdk_lib::NetworkInterface;

namespace huesdk {

using IpCheckFutureVector = std::vector<std::future<BridgeDiscoveryIpCheckResult>>;

    unique_ptr<NetworkInterface> BridgeDiscoveryMethodUtil::get_first_private_network_interface() {
        unique_ptr<NetworkInterface> network_interface;
        
        HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryMethodUtil: get network interfaces" << HUE_ENDL;

        // Get all network interfaces
        const std::vector<NetworkInterface>& network_interfaces = NETWORK::get_network_interfaces();
        
        HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryMethodUtil: network interfaces retrieved; check results" << HUE_ENDL;
        
        for (NetworkInterface network_interface_it : network_interfaces) {
            HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryMethodUtil: ip: " << network_interface_it.get_ip() << ", name: " << network_interface_it.get_name() << HUE_ENDL;
        
            if (network_interface_it.get_inet_type() == INET_IPV4
                && !network_interface_it.is_loopback()
                &&  network_interface_it.is_up()
#ifdef __APPLE__
                &&  network_interface_it.get_name().find("en") != string::npos
#endif
                ) {
                // Check if fallback has already been set
                if (network_interface == nullptr) {
                    // Copy the network interface
                    network_interface = unique_ptr<NetworkInterface>(new NetworkInterface(network_interface_it));
                    
                    if (network_interface_it.is_private()) {
                        // We alraedy have the first private network interface
                        break;
                    }
                // Fallback has been set, but this network interface might meet criteria 4
                } else if (network_interface_it.is_private()) {
                    network_interface = unique_ptr<NetworkInterface>(new NetworkInterface(network_interface_it));
                
                    break;
                }
            }
        }

        return network_interface;
    }

    std::vector<std::string> BridgeDiscoveryMethodUtil::list_ips_from_subnets(const std::vector<std::string>& subnets) {
        std::vector<std::string> ips;

        for (auto& subnet : subnets) {
            size_t pos = subnet.find_last_of(".");

            if (pos == string::npos) {
                continue;
            }

            string ip_base = subnet.substr(0, pos + 1);

            for (unsigned int i = bridge_discovery_const::IPSCAN_IP_RANGE_BEGIN; i <= bridge_discovery_const::IPSCAN_IP_RANGE_END; i++) {
                ips.push_back(ip_base + huesdk_lib::to_string(i));
            }
        }

        return ips;
    }

    string BridgeDiscoveryMethodUtil::get_unique_bridge_id_from_mac(const string& mac) {
        // Strip mac address from ':' characters
        string unique_bridge_id(mac);

        unique_bridge_id.erase(std::remove(unique_bridge_id.begin(),  unique_bridge_id.end(), ':'),  unique_bridge_id.end());
        
        if (unique_bridge_id.length() == 12) {
            unique_bridge_id = huesdk_lib::to_upper_case(unique_bridge_id);
            // Build up the mac address with dots
            unique_bridge_id.insert(6, "FFFE");
        }
        
        return unique_bridge_id;
    }

    BridgeDiscoveryIpCheckResult BridgeDiscoveryMethodUtil::parse_bridge_config_result(const string& ip, const string& result) {
        BridgeDiscoveryIpCheckResult ip_check_result;
        ip_check_result.ip        = ip;
        ip_check_result.reachable = true;
        
        HUE_LOG << HUE_NETWORK << HUE_DEBUG << "BridgeDiscoveryMethodUtil: parse_bridge_config_result: parsing response -> ip: " << ip << ", body: " << result << HUE_ENDL;
        
        JSONNode nodes;
        
        if (!result.empty()) {
            if (libjson::is_valid(result)) {
                nodes = libjson::parse(result);
                // Get unique id
                string unique_id;
                
                JSONNode::const_iterator bridge_id_node_it = nodes.find("bridgeid");
                // Check if bridge id is part of the result
                if (bridge_id_node_it == nodes.end()) {
                    JSONNode::const_iterator mac_node_it = nodes.find("mac");
                    
                    // Convert it to unique id
                    unique_id = mac_node_it != nodes.end() ? BridgeDiscoveryMethodUtil::get_unique_bridge_id_from_mac(mac_node_it->as_string()) : "";
                } else {
                    // Use bridgeid as unique id
                    unique_id = huesdk_lib::to_upper_case(bridge_id_node_it->as_string());
                }
                
                if (unique_id.length() > 0) {
                    // Set the unique id and mark the ip as a bridge
                    ip_check_result.unique_id = unique_id;
                    ip_check_result.is_bridge = true;
                    
                    HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryMethodUtil: parse_bridge_config_result: parsed result -> unique id: " << unique_id << ", ip: " << ip << HUE_ENDL;

                    // Get api version
                    JSONNode::const_iterator api_version_node_it = nodes.find("apiversion");
                    ip_check_result.api_version = api_version_node_it != nodes.end() ? api_version_node_it->as_string() : "";

                    // Get model_id
                    JSONNode::const_iterator model_id_node_it = nodes.find("modelid");
                    ip_check_result.model_id = model_id_node_it != nodes.end() ? model_id_node_it->as_string() : "";
                }
            } else {
                HUE_LOG << HUE_CORE << HUE_ERROR << "BridgeDiscoveryMethodUtil: parse_bridge_config_result: invalid json" << HUE_ENDL;
            }
        }
        
        return ip_check_result;
    }
}  // namespace huesdk
