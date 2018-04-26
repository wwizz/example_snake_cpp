/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

namespace huesdk_jni_core {

    template<typename T>
    struct from {
    };

    template<typename T>
    struct to {
    };

    template <typename From, typename To>
    To convert(const From& value);

    template<typename From, typename To>
    struct base_converter {
        base_converter<From, To>(const From& value) : _value(convert<From, To>(value)) {}
        To value() const { return _value; }
        operator To() const { return _value; }
        To _value;
    };

    template<>
    struct to<void*> : base_converter<int64_t, void*>{                     // NOLINT
        using base_converter<int64_t, void*>::base_converter;              // NOLINT
    };

    template<>
    struct from<void*> {
        using Type = int64_t;                                              // NOLINT
        template<typename T>
        from<void*>(T *ptr) : _value(reinterpret_cast<int64_t>(ptr)) {}     // NOLINT
        Type value() const { return _value; }
        Type _value;
    };

}  // namespace huesdk_jni_core

#define DEFINE_CONVERTER(T1, T2) \
    template<> \
    struct to<T1> : base_converter<T2, T1> { \
        using base_converter<T2, T1>::base_converter; \
    }; \
    template<> \
    struct from<T1> : base_converter<T1, T2> { \
        using base_converter<T1, T2>::base_converter; \
    }

#define DEFINE_DEFAULT_CONVERT_FUNCTION(T1, T2) \
    template<> \
    T2 convert<T1, T2>(const T1& value) { \
        return static_cast<T2>(value); \
    }

