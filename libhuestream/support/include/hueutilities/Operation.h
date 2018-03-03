/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <algorithm>
#include <vector>

#include "hueutilities/Proxy.h"

namespace huesdk {

    class IOperation {
    public:
        virtual ~IOperation() = default;

        virtual void wait() = 0;

        virtual bool is_cancelable() const = 0;
        virtual void cancel() = 0;
    };

    class Operation : public Proxy<IOperation> {
    public:
        using Proxy<IOperation>::Proxy;

        void wait() override {
            call(&IOperation::wait);
        }

        bool is_cancelable() const override {
            return call(&IOperation::is_cancelable).value_or(false);
        }

        void cancel() override {
            call_if([](IOperation* subject){ return subject && subject->is_cancelable(); }, &IOperation::cancel)
                    .or_throw<std::runtime_error>("This operation is not cancelable.");
        }
    };

    class CompositeOperation : public IOperation {
    public:
        void insert(Operation operation) {
            _operations.push_back(operation);
        }

        void wait() override {
            for (auto&& operation : _operations) {
                operation.wait();
            }
        }

        bool is_cancelable() const override {
            return std::all_of(std::begin(_operations), std::end(_operations), [](
                    const Operation& operation) {return operation.is_cancelable();});
        }

        void cancel() override {
            for (auto&& operation : _operations) {
                operation.cancel();
            }
        }

        operator Operation() {
            return Operation{std::make_shared<CompositeOperation>(*this)};
        }

    private:
        std::vector<Operation> _operations;
    };

} //  namespace huesdk
