/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <functional>

namespace huesdk {

    namespace detail {

        struct CallBase {

            template <typename Exception, typename... Ts>
            void or_throw(Ts... args) {
#if !defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 1
                static_assert(std::is_base_of<std::exception, Exception>::value,
                              "Exception has to be derived from std::exception");
                if (!_is_called) {

                    throw Exception(std::forward<Ts>(args)...);
                }
#endif
            }
            bool _is_called = false;
        };

        template <typename T>
        struct Call : CallBase {
            template <typename Subject, typename Method, typename... Ts>
            Call(Subject subject, Method method, Ts&&... args)
                    : Call(true, subject, method, std::forward<Ts>(args)...) {}

            template <typename Subject, typename Method, typename... Ts>
            Call(bool condition, Subject subject, Method method, Ts&&... args) {
                if ((_is_called = condition && subject)) {
                    _result = (subject->*method)(std::forward<Ts>(args)...);
                }
            }

            T value_or(T value) {
                return std::move(_is_called ? _result : value);
            }

            T _result;
        };

        template <>
        struct Call<void> : CallBase {
            template <typename Subject, typename Method, typename... Ts>
            Call(Subject subject, Method method, Ts&&... args)
                    : Call{true, subject, method, std::forward<Ts>(args)...} {}

            template <typename Subject, typename Method, typename... Ts>
            Call(bool condition, Subject subject, Method method, Ts&&... args) {
                if ((_is_called = condition && subject)) {
                    (subject->*method)(std::forward<Ts>(args)...);
                }
            }
        };
    } //  namespace detail

    template <typename Interface>
    class Proxy : public Interface {
    public:
        Proxy() = default;

        explicit Proxy(std::shared_ptr<Interface> subject) : _subject{subject}{}

        template <typename Method, typename... Ts>
        auto call(Method method, Ts&&... args) const
            -> detail::Call<decltype((static_cast<Interface*>(nullptr)->*method)(std::forward<Ts>(args)...))> {
            using ReturnType = decltype((_subject.get()->*method)(std::forward<Ts>(args)...));
            return detail::Call<ReturnType>{_subject.get(), method, std::forward<Ts>(args)...};
        }

        template <typename Method, typename... Ts>
        auto call_if(std::function<bool(Interface*)> condition, Method method, Ts&&... args) const
            -> detail::Call<decltype((static_cast<Interface*>(nullptr)->*method)(std::forward<Ts>(args)...))>{
            using ReturnType = decltype((_subject.get()->*method)(std::forward<Ts>(args)...));
            const bool call_condition = _subject && condition(_subject.get());
            return detail::Call<ReturnType>{call_condition, _subject.get(), method, std::forward<Ts>(args)...};
        }

    private:
        std::shared_ptr<Interface> _subject;
    };

} //  namespace huesdk
