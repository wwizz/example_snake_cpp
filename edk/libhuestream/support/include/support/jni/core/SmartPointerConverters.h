/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <utility>

#include "Converter.h"
#include "JObjectFactory.h"
#include "ClassInfo.h"
#include "Functional.h"

namespace huesdk_jni_core {

    template<typename T>
    struct from<std::shared_ptr<T>> {
        using Type = jobject;

        from<std::shared_ptr<T>>(std::shared_ptr<T> value) {
            _value = create_jobject(value);
            if (_value == nullptr) {
                _value = convert<std::shared_ptr<T>, jobject>(value);
            }
        }

        Type value() const { return _value; }

        operator Type() const { return _value; }

        Type _value = nullptr;
    };

    template<typename T>
    struct to<std::shared_ptr<T>> {
        using Type = std::shared_ptr<T>;

        to<std::shared_ptr<T>>(jobject instance) {
            auto proxy = get_proxy<T>(instance);
            if (proxy != nullptr) {
                _value = proxy->take_shared_ptr();
            }
        }

        Type value() const { return _value; }

        operator Type() const { return _value; }

        Type _value;
    };

    template<typename T>
    struct from<std::unique_ptr<T>> {
        using Type = jobject;

        from<std::unique_ptr<T>>(std::unique_ptr<T> value) {
            _value = create_jobject(std::move(value));
        }

        Type value() const { return _value; }

        operator Type() const { return _value; }

        Type _value;
    };

    template<typename T>
    struct to<std::unique_ptr<T>> {
        using Type = std::unique_ptr<T>;

        to<std::unique_ptr<T>>(jobject instance) {
            auto proxy = get_proxy<T>(instance);
            if (proxy != nullptr) {
                _value = proxy->take_unique_ptr();
            }
        }

        Type value() && { return std::move(_value); }

        operator Type() && { return std::move(_value); }

        Type _value;
    };

}  // namespace huesdk_jni_core

