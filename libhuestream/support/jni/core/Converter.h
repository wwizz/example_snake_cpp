/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef JNI_CORE_CONVERTER_H_
#define JNI_CORE_CONVERTER_H_

#include "ConverterCore.h"

#include <jni.h>
#include <string>

namespace huesdk_jni_core {

    DEFINE_CONVERTER(int, jint);
    DEFINE_CONVERTER(int64_t, jlong);
    DEFINE_CONVERTER(bool, jboolean);
    DEFINE_CONVERTER(double, jdouble);
    DEFINE_CONVERTER(std::string, jstring);
    DEFINE_CONVERTER(unsigned int, jint);

}  // namespace huesdk_jni_core

#endif  // JNI_CORE_CONVERTER_H_
