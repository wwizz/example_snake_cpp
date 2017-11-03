/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef UTIL_FACTORY_H_
#define UTIL_FACTORY_H_

#include <utility>
#include <iostream>
#include <functional>

template<typename Product, typename... Args>
Product huesdk_lib_default_factory(Args... args);

namespace huesdk_lib {

    template<typename Product, typename... Args>
    class ScopedFactory;

    template<typename Product, typename... Args>
    class Factory {
        using FactoryMethod = std::function<Product(Args... args)>;

        friend class ScopedFactory<Product, Args...>;

    public:
        static Product Create(Args... args) {
            return instance()._factoryMethod ? instance()._factoryMethod(std::forward<Args>(args)...) : huesdk_lib_default_factory<Product, Args...>(std::forward<Args>(args)...);
        }

        static void SetFactoryMethod(FactoryMethod factoryMethod) {
            instance()._factoryMethod = factoryMethod;
        }

    private:
        static Factory<Product, Args...> &instance() {
            static Factory<Product, Args...> _instance;
            return _instance;
        }

        FactoryMethod _factoryMethod;
    };

    template<typename Product, typename... Args>
    class ScopedFactory {
        using FactoryMethod = std::function<Product(Args... args)>;
    public:
        ScopedFactory() {
            _factoryMethod = Factory<Product, Args...>::instance()._factoryMethod;
        }
        ~ScopedFactory() {
            Factory<Product, Args...>::SetFactoryMethod(_factoryMethod);
        }
    protected:
        FactoryMethod _factoryMethod;
    };

}  // namespace huesdk_lib

#endif  // UTIL_FACTORY_H_
