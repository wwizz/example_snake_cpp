/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <utility>

#include "support/threading/QueueExecutor.h"

namespace support {

    template <typename T>
    void delete_later(T* object) {
        QueueExecutor::global()->execute([object] {
            delete object;
        });
    }

    template <typename T, typename U>
    void delete_later(std::unique_ptr<T, U>&& object) {
        delete_later(new std::shared_ptr<T>(std::move(object)));
    }

    template <typename T>
    void delete_later(std::shared_ptr<T>&& object) {
        delete_later(new std::shared_ptr<T>(std::move(object)));
    }

}  // namespace support

