/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_STREAM_UDPCONNECTOR_H_
#define HUESTREAM_STREAM_UDPCONNECTOR_H_

#include <network/sockets/SocketUdp.h>
#include <huestream/stream/IConnector.h>

#include <memory>

namespace huestream {

    class UdpConnector : public IConnector {
        enum class ConnectionState {
            Disconnected,
            Connected
        };

     public:
        UdpConnector();

        virtual ~UdpConnector();

        bool Connect(BridgePtr bridge, unsigned short port) override;

        void Disconnect() override;

        bool Send(const char *buffer, unsigned int bufferSize) override;

    private:
        std::string RemovePortFromIp(std::string ip) const;

        std::shared_ptr<huesdk_lib::SocketUdp> _socketUdp;
        huesdk_lib::SocketAddress _peerAddress;
        ConnectionState _connectionState;
    };
}  // namespace huestream

#endif  // HUESTREAM_STREAM_UDPCONNECTOR_H_
