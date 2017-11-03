/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef STD_UTIL_IOBSERVER_H_
#define STD_UTIL_IOBSERVER_H_

namespace huesdk_lib {
    template<typename ...Args>
    class IObserver {
    public:
        virtual void update(Args... args) = 0;
    };
}   // namespace huesdk_lib

#endif  // STD_UTIL_IOBSERVER_H_
