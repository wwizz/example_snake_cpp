/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef UTIL_PROVIDER_H_
#define UTIL_PROVIDER_H_

#include <memory>
#include <functional>

template<typename Product>
Product huesdk_lib_default_provider();

namespace huesdk_lib {

    template <typename Product>
    class Provider {
        using ProviderMethod = std::function<Product()>;

    public:
        class Scope {
        public:
            explicit Scope(ProviderMethod providerMethod) {
                _previousProviderMethod = Provider<Product>::instance()._providerMethod;
                Provider<Product>::SetProviderMethod(providerMethod);
            }

            ~Scope() {
                Provider<Product>::SetProviderMethod(_previousProviderMethod);
            }
        private:
            ProviderMethod _previousProviderMethod;
        };

        static Product Get() {
            return instance()._providerMethod ? instance()._providerMethod() : huesdk_lib_default_provider<Product>();
        }

        static void SetProviderMethod(ProviderMethod providerMethod) {
            instance()._providerMethod = providerMethod;
        }

        static std::unique_ptr<Scope> SetProviderMethodInThisScope(ProviderMethod providerMethod) {
            return std::unique_ptr<Scope>{new Scope{providerMethod}};
        }

    private:
        static Provider<Product> &instance() {
            static Provider<Product> _instance;
            return _instance;
        }

        ProviderMethod _providerMethod;
    };

}  // namespace huesdk_lib


#endif  // UTIL_PROVIDER_H_
