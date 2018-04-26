/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>

#include "hueutilities/Proxy.h"

namespace huesdk {

    class ISubscription {
    public:
        virtual ~ISubscription() = default;

        virtual void enable() = 0;
        virtual void disable() = 0;
    };

    class Subscription : public Proxy<ISubscription> {
    public:
        using Proxy::Proxy;

        void enable() override {
            call(&ISubscription::enable);
        }

        void disable() override {
            call(&ISubscription::disable);
        }
    };

} //  namespace huesdk
