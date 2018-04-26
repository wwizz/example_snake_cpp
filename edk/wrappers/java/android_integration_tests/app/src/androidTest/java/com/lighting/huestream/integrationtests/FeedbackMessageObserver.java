package com.lighting.huestream.integrationtests;

import com.lighting.huestream.FeedbackMessage;

public class FeedbackMessageObserver extends FeedBackHandler {
    @Override
    public synchronized void NewFeedbackMessage(FeedbackMessage message) {
        super.NewFeedbackMessage(message);

        if (_isListening && message.GetId().equals(_idToListen)) {
            _wasReceived = true;
            this.notify();
        }
    }

    public void StartListening(FeedbackMessage.Id id) {
        if (!_isListening) {
            _idToListen = id;
            _isListening = true;
        }
    }

    public boolean wasReceived() {
        return _wasReceived;
    }

    private boolean _isListening = false;
    private boolean _wasReceived = false;
    private FeedbackMessage.Id _idToListen;
}
