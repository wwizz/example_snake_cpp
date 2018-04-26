/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#pragma once

#include <jni.h>

#include <memory>
#include <string>

#include "support/logging/Log.h"
#include "support/threading/DetachingOperationalQueue.h"
#include "support/threading/QueueDispatcher.h"

namespace support {
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
         * Global dispatcher for the java user callbacks.
         */
        std::shared_ptr<QueueDispatcher> dispatcher();

        /*
         * Formatting function for the logging
         */
        std::string string_format(const char* fmt, ...);

        // LOGGING MACROS
#       define LOGD(LOG_TAG, FORMAT_STRING) support::log::log << support::log::wrapper << support::log::debug << LOG_TAG << " - " << FORMAT_STRING << support::log::endl;
#       define LOGV(LOG_TAG, FORMAT_STRING) support::log::log << support::log::wrapper << support::log::warn << LOG_TAG << " - " << FORMAT_STRING << support::log::endl;
#       define LOGE(LOG_TAG, FORMAT_STRING) support::log::log << support::log::wrapper << support::log::error << LOG_TAG << " - " << FORMAT_STRING << support::log::endl;
#       define LOGI(LOG_TAG, FORMAT_STRING) support::log::log << support::log::wrapper << support::log::info << LOG_TAG << " - " << FORMAT_STRING << support::log::endl;
#       define LOGD_EXT(LOG_TAG, FORMAT_STRING, ...) support::log::log << support::log::wrapper << support::log::debug << LOG_TAG << " - " << support::jni::string_format(FORMAT_STRING, __VA_ARGS__) << support::log::endl;
#       define LOGV_EXT(LOG_TAG, FORMAT_STRING, ...) support::log::log << support::log::wrapper << support::log::warn << LOG_TAG << " - " << support::jni::string_format(FORMAT_STRING, __VA_ARGS__) << support::log::endl;
#       define LOGE_EXT(LOG_TAG, FORMAT_STRING, ...) support::log::log << support::log::wrapper << support::log::error << LOG_TAG << " - " << support::jni::string_format(FORMAT_STRING, __VA_ARGS__) << support::log::endl;
#       define LOGI_EXT(LOG_TAG, FORMAT_STRING, ...) support::log::log << support::log::wrapper << support::log::info << LOG_TAG << " - " << support::jni::string_format(FORMAT_STRING, __VA_ARGS__) << support::log::endl;
    }  // namespace jni
}  // namespace support

