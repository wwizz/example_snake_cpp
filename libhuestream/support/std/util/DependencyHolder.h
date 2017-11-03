/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef STD_UTIL_DEPENDENCYHOLDER_H_
#define STD_UTIL_DEPENDENCYHOLDER_H_

#include <memory>

namespace huesdk_lib {
    class DependencyHolder {
    public:
        explicit DependencyHolder(std::shared_ptr<void> dependency) : _dependency(dependency) {
        }

        DependencyHolder(DependencyHolder&&) = default;

    private:
        std::shared_ptr<void>_dependency;
    };
}  // namespace huesdk_lib

#endif  // STD_UTIL_DEPENDENCYHOLDER_H_
