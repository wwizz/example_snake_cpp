/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifdef SDK_TEST

#ifndef NETWORK_SOCKETS_BSD__TEST_CMETHODDELEGATOR_H_
#define NETWORK_SOCKETS_BSD__TEST_CMETHODDELEGATOR_H_

#include <memory>

#include "network/sockets/bsd/SocketBsdCompat.h"

using std::shared_ptr;

namespace huesdk_lib {

    class CMethodDelegate {
    public:
        /**
         @see <sys/socket.h>
         */
        virtual SOCKET_ID socket(int domain, int type, int protocol) = 0;

        /**
         @see <sys/socket.h>
         */
        virtual int bind(SOCKET_ID handle, const struct sockaddr* addr, socklen_t addr_size) = 0;
        
        /**
         @see <unistd.h>
         */
        virtual int close(SOCKET_ID handle) = 0;

        /**
         @see <sys/socket.h>
         */
        virtual int connect(SOCKET_ID socket, const struct sockaddr* address, socklen_t address_len) = 0;

        /**
         @see <sys/_select.h>
         */
        virtual int select(SOCKET_ID handle, fd_set* read_flags, fd_set* write_flags, fd_set* error_flags, struct timeval* timeout) = 0;
        
        /**
         @see <sys/socket.h>
         */
        virtual int64_t sendto(SOCKET_ID handle, const void* buffer, size_t buffer_size, int flags, const struct sockaddr* addr, socklen_t addr_size) = 0;

        /**
        @see <sys/socket.h>
        */
        virtual int64_t send(SOCKET_ID handle, const void* buffer, size_t buffer_size, int flags) = 0;

        
        /**
         @see <sys/socket.h>
         */
        virtual int64_t recv(SOCKET_ID handle, void* buffer, size_t buffer_data, int flags) = 0;
        
        /**
         @see <sys/socket.h>
         */
        virtual int getsockopt(SOCKET_ID handle, int level, int name, void* value, socklen_t* value_size) = 0;
        
        /**
         @see <sys/socket.h>
         */
        virtual int setsockopt(SOCKET_ID handle, int level, int name, const void* value, socklen_t value_size) = 0;

        virtual int set_nonblocking(SOCKET_ID handle) = 0;

        virtual int get_errno() = 0;

        /**
         @see <arpa/inet.h>
         */
        virtual int inet_pton(int af, const char* src, void* dst) = 0;
    };
    
    // Default
    class CMethodDelegateImpl : public CMethodDelegate {
    public:
        /**
         @see <sys/socket.h>
         */
        SOCKET_ID socket(int, int, int) override {
            return 0;  // ::socket(domain, type, protocol);
        }

        /**
         @see <sys/socket.h>
         */
        int bind(SOCKET_ID, const struct sockaddr*, socklen_t) override {
            return 0;  // ::bind(handle, addr, addr_size);
        }
        
        /**
         @see <unistd.h>
         */
        int close(SOCKET_ID) override {
            return 0;  // ::closesocket(handle);
        }

        /**
         @see <sys/socket.h>
         */
        int connect(SOCKET_ID, const struct sockaddr*, socklen_t) override {
            return 0;
        }

        /**
         @see <unistd.h>
         */
        int select(SOCKET_ID, fd_set*, fd_set*, fd_set*, struct timeval*) override {
            return 0;  // ::select(handle, read_flags, write_flags, error_flags, timeout);
        }
        
        /**
         @see <sys/socket.h>
         */
        int64_t sendto(SOCKET_ID, const void*, size_t, int, const struct sockaddr*, socklen_t) override {
            return 0;  // ::sendto(handle, buffer, buffer_size, flags, addr, addr_size);
        }

        /**
        @see <sys/socket.h>
        */
        int64_t send(SOCKET_ID, const void*, size_t, int) override {
            return 0;  // ::send(handle, buffer, buffer_size, flags);
        }

        /**
         @see <sys/socket.h>
         */
        int64_t recv(SOCKET_ID, void*, size_t, int) override {
            return 0;  // ::recv(handle, buffer, buffer_size, flags);
        }
        
        /**
         @see <sys/socket.h>
         */
        int getsockopt(SOCKET_ID, int, int, void*, socklen_t*) override {
            return 0;  // ::getsockopt(handle, level, name, value, value_size);
        }
        
        /**
         @see <sys/socket.h>
         */
        int setsockopt(SOCKET_ID, int, int, const void*, socklen_t) override {
            return 0;  // ::setsockopt(handle, level, name, value, value_size);
        }

        int set_nonblocking(SOCKET_ID) override {
            return 0;
        }

        int get_errno() override {
            return 0;
        }

        /**
         @see <arpa/inet.h>
         */
        int inet_pton(int, const char*, void*) override {
            return 0;
        }
    };

    class CMethodDelegator {
    public:
        /**
         @see <sys/socket.h>
         */
        static SOCKET_ID socket(int domain, int type, int protocol);

        /**
         @see <sys/socket.h>
         */
        static int bind(SOCKET_ID handle, const struct sockaddr* addr, socklen_t addr_size);

        /**
         @see <unistd.h>
         */
        static int close(SOCKET_ID handle);

        /**
         @see <sys/socket.h>
         */
        static int connect(SOCKET_ID socket, const struct sockaddr* address, socklen_t address_len);
        
        /**
         @see <sys/_select.h>
         */
        static int select(SOCKET_ID handle, fd_set* read_flags, fd_set* write_flags, fd_set* error_flags, struct timeval* timeout);
        
        /**
         @see <sys/socket.h>
         */
        static int64_t sendto(SOCKET_ID handle, const void* buffer, size_t buffer_size, int flags, const struct sockaddr* addr, socklen_t addr_size);

        /**
        @see <sys/socket.h>
        */
        static int64_t send(SOCKET_ID handle, const void* buffer, size_t buffer_size, int flags);

        /**
         @see <sys/socket.h>
         */
        static int64_t recv(SOCKET_ID handle, void* buffer, size_t buffer_data, int flags);
        
        /**
         @see <sys/socket.h>
         */
        static int getsockopt(SOCKET_ID handle, int level, int name, void* value, socklen_t* value_size);
        
        /**
         @see <sys/socket.h>
         */
        static int setsockopt(SOCKET_ID handle, int level, int name, const void* value, socklen_t value_size);
        
        static int set_nonblocking(SOCKET_ID handle);

        static int get_errno();

        /**
         @see <arpa/inet.h>
         */
        static int inet_pton(int af, const char* src, void* dst);

        /* delegate */
        
        /**
         Set the delegate
         @note   Initially CMethodDelegateImpl is set as delegate
         @return The delegate, nullptr if no delegate has been set
         */
        static void set_delegate(shared_ptr<CMethodDelegate> delegate);
    };

}  // namespace huesdk_lib

#endif  // NETWORK_SOCKETS_BSD__TEST_CMETHODDELEGATOR_H_

#endif  // SDK_TEST  // NOLINT
