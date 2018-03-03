/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#pragma once

namespace support {
    template<typename ...Args>
    class IObserver {
    public:
        virtual void update(Args... args) = 0;
    };
}   // namespace support

