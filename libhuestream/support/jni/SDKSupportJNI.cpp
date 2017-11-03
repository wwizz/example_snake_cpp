/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <memory>
#include <string>

#include "jni/SDKSupportJNI.h"
#include "jni/JNIConstants.h"

// logging tag
#define TAG "JNI_UTILITIES"

namespace huesdk_lib {
   namespace jni {

       jclass g_cls_ref_tools;
       jclass g_cls_ref_httprequest;
       jclass g_cls_ref_map_entry;

       std::string string_format(const char* fmt, ...) {
           // format the string
           int n = (static_cast<int>(strlen(fmt))) * 2; /* Reserve two times as much as the length of the fmt_str */
           std::string str;
           std::unique_ptr<char[]> formatted;
           va_list ap;
           while (1) {
               formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
               strcpy(&formatted[0], fmt);  // NOLINT(runtime/printf)
               va_start(ap, fmt);
               int final_n = vsnprintf(&formatted[0], static_cast<size_t>(n), fmt, ap);
               va_end(ap);
               if (final_n < 0 || final_n >= n)
                   n += abs(final_n - n + 1);
               else
                   break;
           }
           return std::string(formatted.get());
       }

        // Global vars
        JavaVM* g_vm;  // global reference to JVM

        void init(JavaVM* vm) {
            g_vm = vm;

            JNIEnv* env = getJNIEnv();

            /* tools and helpers */
            g_cls_ref_tools        = static_cast<jclass>(env->NewGlobalRef(env->FindClass(JNI_SDK_CLASS_NATIVE_TOOLS)));
            g_cls_ref_map_entry    = static_cast<jclass>(env->NewGlobalRef(env->FindClass(JNI_JAVA_CLASS_MAP_ENTRY)));
        }

       JavaVM *getJavaVM() {
            return g_vm;
        }

       JNIEnv *getJNIEnv(bool *has_attached) {
            JNIEnv *g_env;
#ifdef ANDROID
            JNIEnv ** j_env = &g_env;
#else
            void **j_env = (void **) &g_env;  // NOLINT(readability/casting)
#endif

            int getEnvStat = g_vm->GetEnv((void **) &g_env, JNI_VERSION_1_6);  // NOLINT(readability/casting)
            if (getEnvStat == JNI_EDETACHED) {
                if (g_vm->AttachCurrentThread(j_env, NULL) != 0) {
                    LOGE(TAG, "Could not attach thread to vm!");
                    return nullptr;
                } else {
                    if (has_attached != nullptr) {
                        *has_attached = true;
                    }
                    return g_env;
                }
            } else if (getEnvStat == JNI_OK) {
                return g_env;
            } else if (getEnvStat == JNI_EVERSION) {
                // not supported
                LOGE(TAG, "JNI version does not support thread attaching - stop");
                return nullptr;
            }
            LOGE_EXT(TAG, "Unknown environment status: %i, - no JNIEnv", getEnvStat);
            return nullptr;  // unknown end
        }

       std::shared_ptr<DetachingOperationalQueue> dispatch_queue() {
           static auto s_isntance = std::make_shared<DetachingOperationalQueue>("java-dispatch");
           return s_isntance;
       }
   }  // namespace jni
}  // namespace huesdk_lib
