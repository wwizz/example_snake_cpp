/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <functional>
#include <memory>

template <typename T>
struct default_object {
    static T get();
};

namespace support {
    template <typename T>
    struct is_default {
        static bool get(const T& data) {
            return data == T();
        }
    };

    template <typename Product, typename ...Args>
    struct is_default<std::function<Product(Args...)>> {
        static bool get(const std::function<Product(Args...)>& data) {
            return !data;
        }
    };

    template <typename T>
    class Provider {
    public:
        using type = T;

        static T get() {
            if (is_default<T>::get(instance()._data)) {
                return default_object<T>::get();
            } else {
                return instance()._data;
            }
        }

        static void set(T data) {
            instance()._data = data;
        }

    private:
        Provider() = default;

        static Provider& instance() {
            static Provider _instance;
            return _instance;
        }
        T _data;
    };

    template <typename T>
    class ScopedProvider {
    public:
        using type = typename Provider<T>::type;
        using provider_type = Provider<T>;

        explicit ScopedProvider(const T& data) {
            _original = provider_type::get();
            provider_type::set(data);
        }

        ~ScopedProvider() {
            Provider<T>::set(_original);
        }

        ScopedProvider(const ScopedProvider&) = delete;
        ScopedProvider(ScopedProvider&&) = delete;

    private:
        T _original;
    };

}  // namespace support
