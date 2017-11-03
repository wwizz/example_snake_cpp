/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "ClassInfo.h"
#include "ArrayList.h"

REGISTER_CLASS(huesdk_jni_core::ArrayList, "java/util/ArrayList");

#include "jni/core/Converter.h"
#include "jni/core/ContainerConverters.h"
#include "jni/core/JNIEnvFactory.h"

namespace huesdk_jni_core {

DEFINE_DEFAULT_CONVERT_FUNCTION(bool, jboolean);
DEFINE_DEFAULT_CONVERT_FUNCTION(int, jint);
DEFINE_DEFAULT_CONVERT_FUNCTION(double, jdouble);
DEFINE_DEFAULT_CONVERT_FUNCTION(long, int);  // NOLINT
DEFINE_DEFAULT_CONVERT_FUNCTION(long long, long);  // NOLINT
DEFINE_DEFAULT_CONVERT_FUNCTION(long long, long long);  // NOLINT
DEFINE_DEFAULT_CONVERT_FUNCTION(long, long);  // NOLINT
DEFINE_DEFAULT_CONVERT_FUNCTION(long, long long);  // NOLINT
DEFINE_DEFAULT_CONVERT_FUNCTION(unsigned int, jint);
DEFINE_DEFAULT_CONVERT_FUNCTION(jint, unsigned int);

template<> 
bool convert<unsigned char, bool>(const unsigned char& value) {  // NOLINT
    return value != 0;
}

template<>
std::string convert<jstring, std::string>(const jstring& value) {
    std::string return_value;
    auto env = JNIEnvFactory::Create();
    if (env) {
        const char *text = env->GetStringUTFChars(value, 0);
        return_value = text;
        env->ReleaseStringUTFChars(value, text);
    }

    return return_value;
}

template <>
jstring convert<std::string, jstring>(const std::string& instance) {
    jstring return_value = nullptr;
    auto env = JNIEnvFactory::Create();
    if (env) {
        return_value = env->NewStringUTF(instance.c_str());
    }
    return return_value;
}

}  // namespace huesdk_jni_core