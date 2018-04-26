/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <functional>
#include <utility>

#include "Provider.h"

template <typename Product, typename ...Args>
std::unique_ptr<Product> huesdk_lib_default_factory(Args... args);

template <typename Product, typename ...Args>
struct default_object<std::function<std::unique_ptr<Product>(Args...)>> {
    static std::function<std::unique_ptr<Product>(Args...)> get() {
        return huesdk_lib_default_factory<Product, Args...>;
    }
};

namespace support {

    template <typename Product, typename ...Args>
    class Factory;

    template <typename Product>
    class Factory<Product> {
    public:
        using product = Product;
        using FactoryMethod = std::function<std::unique_ptr<Product>()>;

        static std::unique_ptr<Product> create() {
            auto factory_method = Provider<FactoryMethod>::get();
            return factory_method();
        }

        static void set_factory_method(
                std::function<std::unique_ptr<Product>()> factory_method) {
            Provider<FactoryMethod>::set(factory_method);
        }
    };

    template <typename Product, typename Head, typename ...Tails>
    class Factory<Product, Head, Tails...> {
    public:
        using product = Product;
        using FactoryMethod = std::function<std::unique_ptr<Product>(Head, Tails...)>;

        static std::unique_ptr<Product> create(Head head, Tails... tails) {
            auto factory_method = Provider<FactoryMethod>::get();
            return factory_method(std::forward<Head>(head), std::forward<Tails>(tails)...);
        }

        static void set_factory_method(
                std::function<std::unique_ptr<Product>(Head, Tails...)> factory_method) {
            Provider<FactoryMethod>::set(factory_method);
        }

        static void set_factory_method(
                std::function<std::unique_ptr<Product>()> f) {
            auto wrapper_lambda = [f] (Head, Tails...) { return f(); };
            Provider<FactoryMethod>::set(wrapper_lambda);
        }
    };

    template <typename Product, typename ...Args>
    class ScopedFactory;

    template <typename Product>
    class ScopedFactory<Product> : public ScopedProvider<std::function<std::unique_ptr<Product>()>> {
    public:
        using factory = Factory<Product>;

        explicit ScopedFactory(const std::function<std::unique_ptr<Product>()>& f)
                : ScopedProvider<std::function<std::unique_ptr<Product>()>>(f) {}
    };

    template <typename Product, typename Head, typename ...Tails>
    class ScopedFactory<Product, Head, Tails...>
            : public ScopedProvider<std::function<std::unique_ptr<Product>(Head, Tails...)>> {
    public:
        using factory = Factory<Product, Head, Tails...>;

        explicit ScopedFactory(const std::function<std::unique_ptr<Product>(Head, Tails...)>& f)
                : ScopedProvider<std::function<std::unique_ptr<Product>(Head, Tails...)>>(f) {}

        explicit ScopedFactory(const std::function<std::unique_ptr<Product>()>& f)
                : ScopedProvider<std::function<std::unique_ptr<Product>(Head, Tails...)>>(
                [f](Head, Tails...) {return f();}) {}
    };

}  // namespace support

