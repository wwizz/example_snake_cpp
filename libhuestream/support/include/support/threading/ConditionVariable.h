/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <mutex>
#include <condition_variable>

namespace support {

    enum class NotifyMode {
        All,
        One
    };

    template <typename T>
    class ConditionVariable {
    public:
        explicit ConditionVariable(const T& value) : _value(value) {}

        void set_value(const T& value, NotifyMode mode = NotifyMode::All) {
            std::lock_guard<std::mutex> lock{_mutex};
            _value = value;
            notify(mode);
        }

        T get_value() const {
            std::lock_guard<std::mutex> lock{_mutex};
            return _value;
        }

        void perform(std::function<void(T&)> operation, NotifyMode mode = NotifyMode::All) {
            std::lock_guard<std::mutex> lock{_mutex};
            operation(_value);
            notify(mode);
        }

        void wait(const T& value) {
            std::unique_lock<std::mutex> lock{_mutex};
            _condition.wait(lock, [&]() {return _value == value;});
        }

        void wait(std::function<bool(const T&)> matcher) {
            std::unique_lock<std::mutex> lock{_mutex};
            _condition.wait(lock, [&]() {return matcher(_value);});
        }

        template< class Rep, class Period>
        bool wait_for(const T& value, const std::chrono::duration<Rep, Period>& rel_time) {
            std::unique_lock<std::mutex> lock{_mutex};
            return _condition.wait_for(lock, rel_time, [&]() {return value == _value;});
        }

        template< class Rep, class Period>
        bool wait_for(std::function<bool(const T&)> matcher, const std::chrono::duration<Rep, Period>& rel_time) {
            std::unique_lock<std::mutex> lock{_mutex};
            return _condition.wait_for(lock, rel_time, [&]() {return matcher(_value);});
        }

        template< class Clock, class Duration >
        bool wait_until(const T& value, const std::chrono::time_point<Clock, Duration>& timeout_time) {
            std::unique_lock<std::mutex> lock{_mutex};
            return _condition.wait_until(lock, timeout_time, [&]() {return value == _value;});
        }

        template< class Clock, class Duration >
        bool wait_until(std::function<bool(const T&)> matcher, const std::chrono::time_point<Clock, Duration>& timeout_time) {
            std::unique_lock<std::mutex> lock{_mutex};
            return _condition.wait_until(lock, timeout_time, [&]() {return matcher(_value);});
        }

    private:
        void notify(NotifyMode mode) {
            if (mode == NotifyMode::All) {
                _condition.notify_all();
            } else {
                _condition.notify_one();
            }
        }

        mutable std::mutex _mutex;
        T _value;
        std::condition_variable _condition;
    };

    namespace operations {
        template <typename T>
        void increment(T& value) {
            ++value;
        }

        template <typename T>
        void decrement(T& value) {
            --value;
        }

        template <typename T>
        std::function<void(T&)> add(const T& value) {
            return std::bind([](T& member, const T& value){member += value;}, std::placeholders::_1, value);
        }

        template <typename T>
        std::function<void(T&)> subtract(const T& value) {
            return std::bind([](T& member, const T& value){member -= value;}, std::placeholders::_1, value);
        }
    }  //  namespace operations

    namespace matchers {
        template <typename T>
        std::function<bool(const T&)> greater_than(const T& value) {
            return std::bind([](const T& member, const T& value){return member > value;}, std::placeholders::_1, value);
        }

        template <typename T>
        std::function<bool(const T&)> less_than(const T& value) {
            return std::bind([](const T& member, const T& value){return member < value;}, std::placeholders::_1, value);
        }
    }  //  namespace matchers

}  //  namespace support
