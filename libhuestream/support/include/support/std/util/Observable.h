/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#pragma once

#include <vector>
#include "support/std/util/IObserver.h"

namespace support {

    template<typename... Args>
    class Observable {
    public:
        Observable() = default;

        virtual ~Observable() = default;

        void add_observer(IObserver<Args...> *observer) {
            _observers.push_back(observer);
        }

        void delete_observer(IObserver<Args...> *observer) {
            _observers.erase(std::remove(_observers.begin(), _observers.end(), observer));
        }

        void notify_observers(Args... args) const {
            for (const auto &observer : _observers) {
                observer->update(args...);
            }
        }

        Observable(const Observable &) = delete;
        Observable &operator=(const Observable &) = delete;

    private:
        std::vector<IObserver<Args...>*> _observers;
    };

}  // namespace support
