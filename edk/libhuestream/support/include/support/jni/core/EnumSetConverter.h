/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <vector>
#include <memory>

#include "hueutilities/EnumSet.h"

#include "Converter.h"
#include "ClassInfo.h"
#include "Functional.h"

namespace huesdk_jni_core {

    template<typename T>
    struct from<huesdk::EnumSet<T>> {
        using Type = jobject;

        explicit from(huesdk::EnumSet<T> value) {
            auto env = JNIEnvFactory::Create();
            jmethodID jconverter = env->GetStaticMethodID(huesdk_jni_core::EnumInfo<T>::Class, "enumSetFromValue", "(J)Ljava/util/EnumSet;");
            _value = env.get()->CallStaticObjectMethod(huesdk_jni_core::EnumInfo<T>::Class, jconverter, static_cast<int>(value));
        }

        Type value() const { return _value; }
        operator Type() const { return _value; }
        Type _value;
    };

    template<typename T>
    struct to<huesdk::EnumSet<T>> {
        using Type = huesdk::EnumSet<T>;

        explicit to(jobject instance) {
            auto env = huesdk_jni_core::JNIEnvFactory::Create();
            jmethodID methodId = env->GetStaticMethodID(huesdk_jni_core::EnumInfo<T>::Class, "valueFromEnumSet",
                                                        "(Ljava/util/EnumSet;)J");
            auto v = env.get()->CallStaticLongMethod(huesdk_jni_core::EnumInfo<T>::Class, methodId, instance);

            _value = Type(static_cast<int>(v));
        }

        Type value() const { return _value; }

        operator Type() const { return _value; }

        Type _value;
    };
}  // namespace huesdk_jni_core
