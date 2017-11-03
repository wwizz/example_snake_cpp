/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MESSAGEDISPATCHERSTUB_H
#define LIBHUESTREAM_MESSAGEDISPATCHERSTUB_H

#include <huestream/connect/IMessageDispatcher.h>
#include <vector>

using std::vector;
using huestream::IMessageDispatcher;


class StubMessageDispatcher : public IMessageDispatcher {
public:
    StubMessageDispatcher();

    virtual void Execute(bool useThisTread) override;

    virtual void Stop() override;

    vector<DispatchAction> _queue;

    virtual void Queue(DispatchAction action) override;

    void ExecutePendingActions();

    bool _isProcessingQueue;
};


#endif  // LIBHUESTREAM_MESSAGEDISPATCHERSTUB_H
