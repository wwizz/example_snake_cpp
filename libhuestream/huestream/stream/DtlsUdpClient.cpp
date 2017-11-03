/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/stream/DtlsUdpClient.h>
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>
#include <mbedtls/net.h>

#include <assert.h>

#include <memory>
#include <string>

namespace huestream {

void DtlsUdpClient::Init() {
    socketUdp_ = std::make_shared<huesdk_lib::SocketUdp>(huesdk_lib::SocketAddress("0.0.0.0", 0));
}

void DtlsUdpClient::Free() {
    socketUdp_->close();
}

netstatus_e DtlsUdpClient::Connect(const std::string& host, unsigned int port) {
    remote_.set_ip(host);
    remote_.set_port(port);
    connected = true;
    if (socketUdp_->bind() != huesdk_lib::SOCKET_STATUS_OK)
        return MBEDTLS_ERR_NET_BIND_FAILED;
    if (socketUdp_->connect_sync(remote_) != huesdk_lib::SOCKET_STATUS_OK)
        return MBEDTLS_ERR_NET_CONNECT_FAILED;

    return 0;
}

int DtlsUdpClient::Send(const unsigned char *buf, size_t len) {
    assert(connected);
    return static_cast<int>(socketUdp_->sync_send(buf, len));
}

int DtlsUdpClient::ReceiveWithTimeout(unsigned char *buf, size_t len, uint32_t timeout) {
    auto retcode = socketUdp_->sync_receive(buf, len, timeout);
    switch (retcode) {
        case huesdk_lib::SOCKET_STATUS_AGAIN:return MBEDTLS_ERR_SSL_WANT_READ;
        case huesdk_lib::SOCKET_STATUS_TIMEOUT:return MBEDTLS_ERR_SSL_TIMEOUT;
        default:return static_cast<int>(retcode);
    }
}
}  // namespace huestream
