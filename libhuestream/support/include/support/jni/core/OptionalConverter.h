/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>
#include <boost/optional.hpp>

namespace huesdk_jni_core {

    template<typename T>
    struct from<boost::optional<T>> {
        using Type = jobject;
        from<boost::optional<T>>(const boost::optional<T>& value) {
            if (value == boost::none) {
                _value = nullptr;
            } else {
                _value = from<T>(value.value()).value();
            }
        }
        Type value() const { return _value; }
        operator Type() const { return _value; }
        Type _value;
    };

    template<typename T>
    struct to<boost::optional<T>> {
        using Type = boost::optional<T>;
        to<boost::optional<T>>(jobject value) {
            if (value == nullptr) {
                _value = boost::none;
            } else {
                _value = to<T>(value).value();
            }
        }
        Type value() const { return _value; }
        operator Type() const { return _value; }
        Type _value;
    };

    template<>
    struct from<boost::optional<std::string>> {
        using Type = jstring;
        from<boost::optional<std::string>>(const boost::optional<std::string>& value) {
            if (value == boost::none) {
                _value = nullptr;
            } else {
                _value = from<std::string>(value.value()).value();
            }
        }
        Type value() const { return _value; }
        operator Type() const { return _value; }
        Type _value;
    };

    template<>
    struct to<boost::optional<std::string>> {
        using Type = boost::optional<std::string>;
        to<boost::optional<std::string>>(jstring value) {
            if (value == nullptr) {
                _value = boost::none;
            } else {
                _value = to<std::string>(value).value();
            }
        }
        Type value() const { return _value; }
        operator Type() const { return _value; }
        Type _value;
    };

    template<>
    struct from<boost::optional<int>> {
        using Type = jobject;
        from<boost::optional<int>>(const boost::optional<int>& value) {
            if (value == boost::none) {
                _value = nullptr;
            } else {
                auto env = JNIEnvFactory::Create();
                static jclass cls = static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/lang/Integer")));
                static jmethodID methodID = env->GetMethodID(cls, "<init>", "(I)V");
                _value = env->NewObject(cls, methodID, value.value());
            }
        }
        Type value() const { return _value; }
        operator Type() const { return _value; }
        Type _value;
    };

    template<>
    struct to<boost::optional<int>> {
        using Type = boost::optional<int>;
        to<boost::optional<int>>(jobject value) {
            if (value == nullptr) {
                _value = boost::none;
            } else {
                auto env = JNIEnvFactory::Create();
                static jclass cls = static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/lang/Integer")));
                static jmethodID methodID = env->GetMethodID(cls,  "intValue", "()I");
                _value = env->CallIntMethod(value, methodID);
            }
        }
        Type value() const { return _value; }
        operator Type() const { return _value; }
        Type _value;
    };

}  // namespace huesdk_jni_core
