package com.lighting.huestream.integrationtests;

import com.lighting.huestream.ConnectResult;
import com.lighting.huestream.FeedbackMessage;
import com.lighting.huestream.StreamingMode;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

@RunWith(JUnit4.class)
public class ConnectivityTests extends BaseTest {

    @BeforeClass
    public static void loadNativeLibrary() {
        System.loadLibrary("huestream_java_native");
    }

    @Before
    public void setUp() {
        threadWaitFor(100);

        _bridge = initializeBridge();
        _bridgeWrapperHelper = initializeBridgeWrapper();

        resetStream();

        threadWaitFor(100);
    }

    @After
    public void tearDown() {
        threadWaitFor(100);
        _bridgeWrapperHelper.cleanUpUser();
        _hue_stream.ShutDown();

        threadWaitFor(100);

        _hue_stream = null;
        _bridge = null;
        _bridgeWrapperHelper = null;

        threadWaitFor(100);
    }

    private void checkStreamConnectionValid() {
        Assert.assertEquals("Connection result must be Streaming", ConnectResult.Streaming, _hue_stream.GetConnectionResult());
        Assert.assertTrue("Bridge is not authorized", _hue_stream.GetActiveBridge().IsAuthorized());
        Assert.assertTrue("Bridge is not streaming", _hue_stream.IsBridgeStreaming());
    }

    private void checkStreamNotConnected() {
        Assert.assertEquals("Connection result must be Unitialized", ConnectResult.Uninitialized, _hue_stream.GetConnectionResult());
        Assert.assertFalse("Bridge is still authorized", _hue_stream.GetActiveBridge().IsAuthorized());
        Assert.assertFalse("Bridge is still streaming", _hue_stream.IsBridgeStreaming());
        Assert.assertFalse("Bridge is still connectable", _hue_stream.GetActiveBridge().IsConnectable());
    }

    private void resetStream() {
        if ((_hue_stream != null) && _hue_stream.IsBridgeStreaming())
        {
            _hue_stream.ShutDown();
        }

        threadWaitFor(100);

        _hue_stream = createStream(StreamingMode.STREAMING_MODE_DTLS);
        _hue_stream.RegisterFeedbackHandler(new FeedBackHandler());
    }


    @Test
    public void TestBridgeConnectivity_Connect_InvalidClientKey() {
        _bridge.SetClientKey("Invalid Client Key");
        _hue_stream.ConnectManualBridgeInfo(_bridge);

        Assert.assertEquals("Stream should expect action", ConnectResult.ActionRequired, _hue_stream.GetConnectionResult());
        Assert.assertFalse("Client key should not be valid", _hue_stream.GetActiveBridge().IsValidClientKey());
    }

    @Test
    public void TestBridgeConnectivity_Connect_Valid_Async() {
        FeedbackMessageObserver messageObserver = new FeedbackMessageObserver();
        _hue_stream.RegisterFeedbackHandler(messageObserver);

        // Start listening on the ID_USERPROCEDURE_FINISHED message, connect to the bridge
        messageObserver.StartListening(FeedbackMessage.Id.ID_USERPROCEDURE_FINISHED);
        _hue_stream.ConnectManualBridgeInfoAsync(_bridge);

        // Wait for the message
        try {
            synchronized (messageObserver) {
                messageObserver.wait(DEFAULT_TIMEOUT_MS);
                Assert.assertTrue("Wait timeout", messageObserver.wasReceived());
            }
        } catch (InterruptedException e) {
            Assert.fail("Bridge connection interrupted");
        }
        checkStreamConnectionValid();
    }

    @Test
    public void TestBridgeConnectivity_Connect_InvalidUsername_Async() {
        _bridge.SetUser("Invalid Username");

        // Create message observer, connect event handle to it and attach it to the stream
        FeedbackMessageObserver messageObserver = new FeedbackMessageObserver();
        _hue_stream.RegisterFeedbackHandler(messageObserver);

        // Start listening on the FINISH_RETRIEVING_FAILED message, connect to the bridge
        messageObserver.StartListening(FeedbackMessage.Id.ID_FINISH_RETRIEVING_FAILED);
        _hue_stream.ConnectManualBridgeInfoAsync(_bridge);

        // Wait for the message
        try {
            synchronized (messageObserver) {
                messageObserver.wait(DEFAULT_TIMEOUT_MS);
                Assert.assertTrue("Wait timeout", messageObserver.wasReceived());
            }
        } catch (InterruptedException e) {
            Assert.fail("Bridge connection interrupted");
        }
    }

    @Test
    public void TestBridgeConnectivity_Connect_NoEntertainmentGroups_AddGroup_Connect() {
        _bridgeWrapperHelper.removeEntertainmentGroups();
        Assert.assertFalse( "There is still selected group on bridge", _hue_stream.GetActiveBridge().IsGroupSelected());
        _hue_stream.ConnectManualBridgeInfo(_bridge);
        Assert.assertEquals("Stream should expect action", ConnectResult.ActionRequired, _hue_stream.GetConnectionResult());

        // Stop streaming.
        resetStream();
        threadWaitFor(100);
        checkStreamNotConnected();

        Integer entertainmentGroup = _bridgeWrapperHelper.getEntertainmentGroupId();
        _bridge.SelectGroup(entertainmentGroup.toString());
        threadWaitFor(100);

        // Try to connect again
        _hue_stream.ConnectManualBridgeInfo(_bridge);
        threadWaitFor(100);
        checkStreamConnectionValid();
    }

}
