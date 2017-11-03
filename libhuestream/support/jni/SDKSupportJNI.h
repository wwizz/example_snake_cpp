/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef JNI_SDKSUPPORTJNI_H_
#define JNI_SDKSUPPORTJNI_H_

#include <jni.h>

#include <memory>
#include <string>

#include "logging/Log.h"
#include "threading/DetachingOperationalQueue.h"

namespace huesdk_lib {
    namespace jni {

        extern jclass g_cls_ref_tools;
        extern jclass g_cls_ref_httprequest;
        extern jclass g_cls_ref_map_entry;

        void init(JavaVM* vm);

        /*
         * Get Java VM
         */
        JavaVM* getJavaVM();

        /*
         * Get JNI Env
         */
        JNIEnv* getJNIEnv(bool* has_attached = nullptr);

        /*
         * Global execution queue for the java user callbacks.
         */
        std::shared_ptr<DetachingOperationalQueue> dispatch_queue();

        /*
         * Formatting function for the logging
         */
        std::string string_format(const char* fmt, ...);

        // LOGGING MACROS
#       define LOGD(LOG_TAG, FORMAT_STRING) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::debug << LOG_TAG << " - " << FORMAT_STRING << huesdk_lib::log::endl;
#       define LOGV(LOG_TAG, FORMAT_STRING) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::warn << LOG_TAG << " - " << FORMAT_STRING << huesdk_lib::log::endl;
#       define LOGE(LOG_TAG, FORMAT_STRING) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::error << LOG_TAG << " - " << FORMAT_STRING << huesdk_lib::log::endl;
#       define LOGI(LOG_TAG, FORMAT_STRING) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::info << LOG_TAG << " - " << FORMAT_STRING << huesdk_lib::log::endl;
#       define LOGD_EXT(LOG_TAG, FORMAT_STRING, ...) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::debug << LOG_TAG << " - " << huesdk_lib::jni::string_format(FORMAT_STRING, __VA_ARGS__) << huesdk_lib::log::endl;
#       define LOGV_EXT(LOG_TAG, FORMAT_STRING, ...) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::warn << LOG_TAG << " - " << huesdk_lib::jni::string_format(FORMAT_STRING, __VA_ARGS__) << huesdk_lib::log::endl;
#       define LOGE_EXT(LOG_TAG, FORMAT_STRING, ...) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::error << LOG_TAG << " - " << huesdk_lib::jni::string_format(FORMAT_STRING, __VA_ARGS__) << huesdk_lib::log::endl;
#       define LOGI_EXT(LOG_TAG, FORMAT_STRING, ...) huesdk_lib::log::log << huesdk_lib::log::wrapper << huesdk_lib::log::info << LOG_TAG << " - " << huesdk_lib::jni::string_format(FORMAT_STRING, __VA_ARGS__) << huesdk_lib::log::endl;
    }  // namespace jni
}  // namespace huesdk_lib

#endif  // JNI_SDKSUPPORTJNI_H_
