/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "bridgediscovery/method/upnp/tasks/BridgeDiscoveryUpnpTask.h"

#include <string>
#include <vector>
#include <regex>
#include <utility>

#include "BridgeDiscoveryUpnpListenTask.h"
#include "BridgeDiscoveryUpnpSendTask.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"
#include "network/sockets/SocketAddress.h"
#include "network/sockets/SocketError.h"
#include "logging/Log.h"

using std::regex;
using std::regex_search;
using std::smatch;
using std::vector;

using huesdk_lib::JobTask;
using huesdk_lib::JobState;
using huesdk_lib::NestedJob;
using huesdk_lib::Vector;
using huesdk_lib::SocketError;
using huesdk_lib::SocketAddress;
using huesdk_lib::to_string;
using huesdk_lib::Timer;
using huesdk_lib::NetworkInterface;

namespace {
    /** consts for the matched regex results */
    const int IP_POS = 1;
    const int API_VERSION_POS = 2;
    const int MAC_POS = 3;
    const int BRIDGE_ID_POS = 5;

    unique_ptr<SOCKET_UDP> init_socket() {
        unique_ptr<SOCKET_UDP> socket;

        HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryUpnp: initialize socket" << HUE_ENDL;

        {  // Initialisation
            HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryUpnp: get network interface" << HUE_ENDL;

            unique_ptr<NetworkInterface> network_interface = huesdk::BridgeDiscoveryMethodUtil::get_first_private_network_interface();

            if (network_interface != nullptr) {
                HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryUpnp: binding to ip: "
                        << network_interface->get_ip() << HUE_ENDL;

                // Create and open new socket
                unique_ptr<SOCKET_UDP> socket_init = unique_ptr<SOCKET_UDP>(new SOCKET_UDP(SocketAddress(network_interface->get_ip(), 0)));
                // Configure the socket
                socket_init->set_reuse_address(true);
                socket_init->set_broadcast(true);
                socket_init->join_group(SocketAddress(huesdk::bridge_discovery_const::UDP_SOCKET_IP, 0));
                // Bind the socket to the local address
                int socket_status = socket_init->bind();

                HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryUpnp: initialize socket -> join group: "
                        << huesdk::bridge_discovery_const::UDP_SOCKET_IP << HUE_ENDL;

                if (socket_status == huesdk_lib::SOCKET_STATUS_OK) {
                    // Assign the initialized socket
                    socket = move(socket_init);
                }
            } else {
                HUE_LOG << HUE_CORE << HUE_ERROR << "BridgeDiscoveryUpnp: no valid network interface available"
                        << HUE_ENDL;
            }
        }

        HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryUpnp: socket initialized" << HUE_ENDL;

        return socket;
    }

    vector<huesdk::BridgeDiscoveryResult> parse_results(string response) {
        vector<huesdk::BridgeDiscoveryResult> results;

        smatch result;
        // Parse the found bridges from the data string
        while (regex_search(response, result, regex(huesdk::bridge_discovery_const::UPNP_RESPONSE_REGEX))) {
            string ip = string(result[IP_POS].first, result[IP_POS].second);
            string api_version = string(result[API_VERSION_POS].first, result[API_VERSION_POS].second);
            string unique_bridge_id;

            if (result[BRIDGE_ID_POS].matched) {
                unique_bridge_id = huesdk_lib::to_upper_case(string(result[BRIDGE_ID_POS].first, result[BRIDGE_ID_POS].second));
            } else {
                string mac = string(result[MAC_POS].first, result[MAC_POS].second);
                unique_bridge_id = huesdk::BridgeDiscoveryMethodUtil::get_unique_bridge_id_from_mac(mac);
            }

            results.emplace_back(unique_bridge_id, ip, api_version, "");

            // Gets sequence after last submatch.
            response = result.suffix().str();
        }

        // Parse the found bridges, with firmware > 1.9
        while (regex_search(response, result, regex(huesdk::bridge_discovery_const::UPNP_RESPONSE_REGEX_19))) {
            string ip = string(result[IP_POS].first, result[IP_POS].second);
            string api_version = string(result[API_VERSION_POS].first, result[API_VERSION_POS].second);
            string unique_bridge_id = string(result[MAC_POS].first, result[MAC_POS].second);

            results.emplace_back(unique_bridge_id, ip, api_version, "");

            // Gets sequence after last submatch.
            response = result.suffix().str();
        }

        return results;
    }
}  // namespace

namespace huesdk {
    BridgeDiscoveryUpnpTask::BridgeDiscoveryUpnpTask() : _socket(init_socket()) {}

    void BridgeDiscoveryUpnpTask::execute(JobTask::CompletionHandler done) {
        _done = std::move(done);

        if (_socket == nullptr) {
            _done();
        } else {
            _timeout_timer.reset(new Timer(bridge_discovery_const::UDP_TIMEOUT, [this]() {
                HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryUpnp: timeout timer expired, so stop searching"
                        << HUE_ENDL;
                _socket->close();
            }));

            _send_job = huesdk_lib::create_job<BridgeDiscoveryUpnpSendTask>(_socket.get());
            _send_job->run([this](BridgeDiscoveryUpnpSendTask *task) {
                if (task->is_successful()) {
                    start_listen_task();
                } else {
                    _done();
                }
            });

            _timeout_timer->start();
        }
    }

    void BridgeDiscoveryUpnpTask::stop() {
        _timeout_timer->stop();
        _socket->close();

        if (_send_job != nullptr && _send_job->state() == JobState::Running) {
            _send_job->cancel();
        }
    }

    const vector<BridgeDiscoveryResult> &BridgeDiscoveryUpnpTask::get_result() const {
        return _results;
    }

    void BridgeDiscoveryUpnpTask::start_listen_task() {
        auto listen_job = create_job<BridgeDiscoveryUpnpListenTask>(_socket.get());
        listen_job->run([this](BridgeDiscoveryUpnpListenTask *task) {
            if (_socket->is_opened()) {
                _listen_result += task->get_result();
                start_listen_task();
            } else {
                HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryUpnp: parse results: " << _listen_result
                        << HUE_ENDL;
                _results = parse_results(_listen_result);
                _done();
            }
        });
    }
}  // namespace huesdk
