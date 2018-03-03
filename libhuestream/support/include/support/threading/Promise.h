/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <future>

namespace support {

    template <typename T>
    class Promise : public std::promise<T> {
    public:
        Promise() : _future{std::promise<T>::get_future()} {};
        const std::future<T>& get_future() const {
            return _future;
        }

        std::future<T>& get_future() {
            return _future;
        }

    private:
        std::future<T> _future;
    };

}  //  namespace support