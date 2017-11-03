/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef JNI_CORE_PROXY_H_
#define JNI_CORE_PROXY_H_

#include <jni.h>
#include <utility>
#include <string>
#include <mutex>
#include <unordered_map>
#include <memory>

#include "JNIEnvFactory.h"

namespace huesdk_jni_core {

    template <typename T>
    T* clone(const T& instance);

    template<typename T>
    class Proxy {
    public:
        Proxy<T>(T *subject, bool takeOwnership) : _subject(subject), _is_owner(takeOwnership) {}
        Proxy<T>(std::shared_ptr<T> subject) : _subject(subject.get()), _is_owner(false), _shared_ptr(subject) {}
        Proxy<T>(std::unique_ptr<T> subject) : _subject(subject.get()), _is_owner(false), _unique_ptr(std::move(subject)) {}

        ~Proxy<T>() {
            auto env = JNIEnvFactory::Create();
            for (auto iter = _cache.begin(); iter != _cache.end(); ++iter) {
                env->DeleteGlobalRef(iter->second);
            }

            if (_is_owner && _subject) {
                delete _subject;
                _subject = nullptr;
            }
        }

        T *subject() const {
            return _subject;
        }

        std::shared_ptr<T> take_shared_ptr() const {
            std::shared_ptr<T> return_value = _shared_ptr;

            if (return_value.get() == nullptr) {
                return_value.reset(clone<T>(*_subject));
            }

            return return_value;
        }

        std::unique_ptr<T> take_unique_ptr() {
            std::unique_ptr<T> return_value = std::move(_unique_ptr);

            if (return_value.get() == nullptr) {
                return_value.reset(clone<T>(*_subject));
            }

            return return_value;
        }

        jobject get_cache_item(const std::string &key, std::function<jobject()> function) const {
            auto env = JNIEnvFactory::Create();
            std::lock_guard<std::mutex> lock{_cache_mutex};
            auto iter = _cache.find(key);
            if (iter == _cache.end()) {
                _cache[key] = env->NewGlobalRef(function());
            }

            return _cache[key];
        }

    private:
        T* _subject = nullptr;
        bool _is_owner = false;
        std::shared_ptr<T> _shared_ptr;
        std::unique_ptr<T> _unique_ptr;
        mutable std::mutex _cache_mutex;
        mutable std::unordered_map<std::string, jobject> _cache;
    };

}  // namespace huesdk_jni_core

#endif  // JNI_CORE_PROXY_H_
