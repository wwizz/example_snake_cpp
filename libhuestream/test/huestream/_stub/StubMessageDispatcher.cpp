/*******************************************************************************
 Copyright (c) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "StubMessageDispatcher.h"

StubMessageDispatcher::StubMessageDispatcher() :
        _queue(), _isProcessingQueue(false) {
}

void StubMessageDispatcher::Queue(DispatchAction action) {
    _queue.insert(_queue.begin(), action);
}

void StubMessageDispatcher::ExecutePendingActions() {
    while (_queue.size() > 0) {
        _isProcessingQueue = true;
        auto action = _queue.back();
        _queue.pop_back();
        action();
    }
    _isProcessingQueue = false;
}

void StubMessageDispatcher::Execute(bool useThisTread) {

}

void StubMessageDispatcher::Stop() {

}



