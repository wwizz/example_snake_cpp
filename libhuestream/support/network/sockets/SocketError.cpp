/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <string>

#include "network/sockets/SocketError.h"

using std::string;

namespace huesdk_lib {

    SocketError::SocketError(const string& message, int code) : _message(string(message)),
                                                                _code(code) { }

    const string& SocketError::get_message() const {
        return _message;
    }

    void SocketError::set_message(const string& message) {
        _message = string(message);
    }

    int SocketError::get_code() const {
        return _code;
    }

    void SocketError::set_code(int code) {
        _code = code;
    }

}  // namespace huesdk_lib
