/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef SDK_TEST
#include "network/Network.h"
#else
#include "network/_test/Network.h"  // NOLINT(build/include)
#endif

#include "logging/Log.h"


#if defined(CONSOLE_SUPPORT)
#elif defined(_WIN32)
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#elif defined(ANDROID)
#include <jni.h>
#include "jni/SDKSupportJNI.h"
#include "jni/JNIToNative.h"
#else
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <string>
#endif

using std::string;

#ifdef _WIN32
using std::runtime_error;
#endif

namespace huesdk_lib {
    bool Network::_default_network_interface_set = false;
    NetworkInterface Network::_default_network_interface;

    void Network::set_default_network_interface(std::string ip, std::string name, NetworkInetType type) {
        _default_network_interface.set_ip(ip);
        _default_network_interface.set_name(name);
        _default_network_interface.set_inet_type(type);
        _default_network_interface.set_up(true);
        _default_network_interface.set_loopback(false);
        _default_network_interface_set = true;
    }

#if defined(CONSOLE_SUPPORT)
#include "network/console/NetworkConsole.h"
#elif defined(_WIN32)
    const std::vector<NetworkInterface> Network::get_network_interfaces() {
        std::vector<NetworkInterface> network_interfaces;
        if (_default_network_interface_set) {
            network_interfaces.push_back(_default_network_interface);
            return network_interfaces;
        }
        ULONG buflen = sizeof(IP_ADAPTER_INFO);
        IP_ADAPTER_INFO *pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(malloc(buflen));

        if (GetAdaptersInfo(pAdapterInfo, &buflen) == ERROR_BUFFER_OVERFLOW) {
            free(pAdapterInfo);
            pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(malloc(buflen));
        }

        if (GetAdaptersInfo(pAdapterInfo, &buflen) == NO_ERROR) {
            for (IP_ADAPTER_INFO *pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next) {
                string test1 = pAdapter->GatewayList.IpAddress.String;
                if (test1 != "0.0.0.0") {
                    NetworkInterface network_interface;
                    network_interface.set_up(true);
                    network_interface.set_loopback(false);

                    network_interface.set_ip(pAdapter->IpAddressList.IpAddress.String);
                    network_interface.set_inet_type(INET_IPV4);
                    network_interface.set_name(pAdapter->Description);

                    network_interfaces.push_back(network_interface);
                    // printf("%s (%s)\n", pAdapter->IpAddressList.IpAddress.String, pAdapter->Description);
                }
            }
        }

        if (pAdapterInfo) {
            free(pAdapterInfo);
        }

        return network_interfaces;
    }
#elif defined(ANDROID)
    const std::vector<NetworkInterface> Network::get_network_interfaces() {
        std::vector<NetworkInterface> network_interfaces;
        if (_default_network_interface_set) {
            network_interfaces.push_back(_default_network_interface);
            return network_interfaces;
        }
        HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "Network: get all network interfaces (JNI)" << HUE_ENDL;
        
        // collect network interface data from Java, using NetUtil helper class
        // NetUtil helps to protect against Exceptions, and avoids unnecessary c++ code
        
        // get NetUtil instance from NativeTools
        bool has_attached = false;
        JNIEnv* env = jni::getJNIEnv(&has_attached);
        jmethodID j_get_netutil = env->GetStaticMethodID(jni::g_cls_ref_tools, "getNetUtil", "()Lcom/philips/lighting/hue/sdk/wrapper/utilities/NetUtil;");
        jobject j_netutil = env->CallStaticObjectMethod(jni::g_cls_ref_tools, j_get_netutil);
        jclass j_netutil_cls = env->GetObjectClass(j_netutil);
        jclass cls = j_netutil_cls;  // shorthand
        
        // first get number of available network interfaces, so we can start a loop
        jmethodID j_get_if_count = env->GetMethodID(cls, "getInterfaceCount", "()I");
        jint j_interface_count = env->CallIntMethod(j_netutil, j_get_if_count);
        
        // start loop
        for (int i = 0; i < static_cast<int>(j_interface_count); i++) {
            // collect ip, name, type, up, loopback
            // prepare local variables
            string name = "";
            string ip = "";
            NetworkInetType inet_type = INET_IPV4;
           
            // name
            jmethodID j_get_name = env->GetMethodID(cls, "getInterfaceName", "(I)[B");
            jobject j_name_obj = env->CallObjectMethod(j_netutil, j_get_name, i);
            if (j_name_obj != NULL) {
                jbyteArray j_name = static_cast<jbyteArray>(j_name_obj);
                const char* n_name = jni::util::byteArrayToString(env, j_name);
                if (n_name != nullptr) {
                    name = string(n_name);
                    delete n_name;
                    env->DeleteLocalRef(j_name_obj);
                } else {
                    HUE_LOG << HUE_NETWORK <<  HUE_ERROR << "Network: Could not get name for interface from Java, translate failed" << HUE_ENDL;
                }
            } else {
                HUE_LOG << HUE_NETWORK <<  HUE_WARN << "Network: Could not get name for interface from Java, retrieve failed" << HUE_ENDL;
            }

            // ip & type, prefer ipv4, use ipv6 as backup
            jmethodID j_get_ipv4 = env->GetMethodID(cls, "getInterfaceIPv4Address", "(I)[B");
            jobject j_ip_obj = env->CallObjectMethod(j_netutil, j_get_ipv4, i);
            if (j_ip_obj != NULL) {
                // first get ipv4
                jbyteArray j_ip = static_cast<jbyteArray>(j_ip_obj);
                const char* n_ip = huesdk_lib::jni::util::byteArrayToString(env, j_ip);
                if (n_ip != nullptr) {
                    ip = string(n_ip);
                    // leave type at IPv4
                    delete n_ip;
                    env->DeleteLocalRef(j_ip_obj);
                } else {
                    HUE_LOG << HUE_NETWORK <<  HUE_ERROR << "Network: Could not get ipv4 for interface from Java, translate failed" << HUE_ENDL;
                }
            } else {
                // ipv4 failed, get ipv6
                jmethodID j_get_ipv6 = env->GetMethodID(cls, "getInterfaceIPv6Address", "(I)[B");
                j_ip_obj = env->CallObjectMethod(j_netutil, j_get_ipv6, i);
                if (j_ip_obj != NULL) {
                    jbyteArray j_ip = static_cast<jbyteArray>(j_ip_obj);
                    const char* n_ip = huesdk_lib::jni::util::byteArrayToString(env, j_ip);
                    if (n_ip != nullptr) {
                        ip = string(n_ip);
                        // set type to IPv6
                        inet_type = INET_IPV6;
                        delete n_ip;
                        env->DeleteLocalRef(j_ip_obj);
                    } else {
                        HUE_LOG << HUE_NETWORK <<  HUE_ERROR << "Network: Could not get ipv6 for interface from Java, translate failed" << HUE_ENDL;
                    }
                } else {
                    HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "Network: Could not get IP for interface from Java, retieve failed" << HUE_ENDL;
                }
            }
            
            // up
            jmethodID j_get_up = env->GetMethodID(cls, "getInterfaceUp", "(I)Z");
            jboolean j_up_obj = env->CallBooleanMethod(j_netutil, j_get_up, i);
            bool up = static_cast<bool>(j_up_obj);
            
            // loopback
            jmethodID j_get_loopback = env->GetMethodID(cls, "getInterfaceLoopback", "(I)Z");
            jboolean j_loopback_obj = env->CallBooleanMethod(j_netutil, j_get_loopback, i);
            bool loopback = static_cast<bool>(j_loopback_obj);

            // build NetworkInterface struct
            NetworkInterface network_interface;
            network_interface.set_ip(ip);
            network_interface.set_name(name);
            network_interface.set_inet_type(inet_type);
            network_interface.set_up(up);
            network_interface.set_loopback(loopback);
            
            network_interfaces.push_back(network_interface);
        }

        if (has_attached) {
            // detach from JavaVM
            huesdk_lib::jni::getJavaVM()->DetachCurrentThread();
        }

        // return vector
        return network_interfaces;
    }
#else
    const std::vector<NetworkInterface> Network::get_network_interfaces() {
        std::vector<NetworkInterface> network_interfaces;
        
        HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "Network: get all network interfaces" << HUE_ENDL;
        if (_default_network_interface_set) {
            network_interfaces.push_back(_default_network_interface);
            return network_interfaces;
        }

        ifaddrs *if_addr    = nullptr;
        ifaddrs *if_addr_it = nullptr;
        // Get all network interfaces
        int result = getifaddrs(&if_addr);
        
        if (result != -1) {
            HUE_LOG << HUE_NETWORK << HUE_DEBUG << "Network: network interfaces retrieved; check for results" << HUE_ENDL;
            
            for (if_addr_it = if_addr; if_addr_it != nullptr; if_addr_it = if_addr_it->ifa_next) {
                HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "Network: interface found, name: " << if_addr_it->ifa_name << HUE_ENDL;
            
                    // Get inet type
                    NetworkInetType inet_type = if_addr_it->ifa_addr->sa_family == AF_INET ? INET_IPV4 : INET_IPV6;
                    
                    string ip;
                    // Resolve ip address
                    switch (inet_type) {
                        case INET_IPV4: {
                            char ip4[INET_ADDRSTRLEN];
                            
                            in_addr addr4 = reinterpret_cast<sockaddr_in*>(if_addr_it->ifa_addr)->sin_addr;
                            // Convert to ipv4 string
                            inet_ntop(AF_INET, &addr4, ip4, INET_ADDRSTRLEN);
                            
                            ip = string(ip4);
                            break;
                        }
                        case INET_IPV6: {
                            char ip6[INET6_ADDRSTRLEN];
                            
                            in6_addr addr6 = reinterpret_cast<sockaddr_in6*>(if_addr_it->ifa_addr)->sin6_addr;
                            // Convert to ipv6 string
                            inet_ntop(AF_INET6, &addr6, ip6, INET6_ADDRSTRLEN);
                            
                            ip = string(ip6);
                            break;
                        }
                    }
                    
                    HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "Network: network interface found, ip: " << ip << ", name: " << if_addr_it->ifa_name << HUE_ENDL;
                
                    // build NetworkInterface struct
                    NetworkInterface network_interface;
                    network_interface.set_ip(ip);
                    network_interface.set_name(string(if_addr_it->ifa_name));
                    network_interface.set_inet_type(inet_type);
                    network_interface.set_up(if_addr_it->ifa_flags & IFF_UP);
                    network_interface.set_loopback(if_addr_it->ifa_flags & IFF_LOOPBACK);
                    
                    network_interfaces.push_back(network_interface);
            }
            
            freeifaddrs(if_addr);
        } else {
            HUE_LOG << HUE_NETWORK <<  HUE_ERROR << "Network: error retrieving network interfaces" << HUE_ENDL;
        }
        
        return network_interfaces;
    }
#endif

}  // namespace huesdk_lib
