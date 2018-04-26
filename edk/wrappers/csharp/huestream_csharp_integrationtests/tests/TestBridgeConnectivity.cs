using NUnit.Framework;
using huestream;
using System.Threading;

namespace huestream_tests
{
    [TestFixture()]
    public class TestBridgeConnectivity : TestBase
    {
        [SetUp]
        public void SetUp()
        {
            _bridge = InitializeBridge();
            _bridgeWrapperHelper = InitializeBridgeWrapper();

            ResetStream();
        }

        [TearDown]
        public void TearDown()
        {
            CleanupUser();
            _hue_stream.ShutDown();
        }

        public static void CheckStreamConnectionValid(HueStream hueStream)
        {
            Assert.AreEqual(hueStream.GetConnectionResult(), ConnectResult.Streaming, "Connection result must be Streaming");
            Assert.IsTrue(hueStream.GetActiveBridge().IsAuthorized(), "Bridge is not authorized");
            Assert.IsTrue(hueStream.IsBridgeStreaming(), "Bridge is not streaming");
        }

        public static void CheckStreamNotConnected(HueStream hueStream)
        {
            Assert.AreEqual(hueStream.GetConnectionResult(), ConnectResult.Uninitialized, "Connection result must be Unitialized");
            Assert.IsFalse(hueStream.GetActiveBridge().IsAuthorized(), "Bridge is still authorized");
            Assert.IsFalse(hueStream.IsBridgeStreaming(), "Bridge is still streaming");
            Assert.IsFalse(hueStream.GetActiveBridge().IsConnectable(), "Bridge is still connectable");
        }

        private void ResetStream()
        {
            if ((_hue_stream != null) && _hue_stream.IsBridgeStreaming())
            {
                _hue_stream.ShutDown();
            }

            _message_handler = new FeedbackMessageHandler();

            _hue_stream = CreateStream(StreamingMode.STREAMING_MODE_UDP);
            _hue_stream.RegisterFeedbackHandler(_message_handler);
        }

        [Test]
        public void TestBridgeConnectivity_Connect_Valid()
        {
            _hue_stream.ConnectManualBridgeInfo(_bridge);
            CheckStreamConnectionValid(_hue_stream);
        }

        [Test]
        public void TestBridgeConnectivity_Connect_InvalidClientKey()
        {
            _bridge.SetClientKey("Invalid Client Key");
            _hue_stream.ConnectManualBridgeInfo(_bridge);
            Assert.AreEqual(_hue_stream.GetConnectionResult(), ConnectResult.ActionRequired, "Stream should expect action");
            Assert.IsFalse(_hue_stream.GetActiveBridge().IsValidClientKey(), "Client key should not be valid");
        }

        [Test]
        public void TestBridgeConnectivity_Connect_Valid_Async()
        {
            // Create message observer, connect event handle to it and attach it to the stream
            var messageObserver = new FeedbackMessageObserver();
            _message_handler = messageObserver;

            var waitHandle = new AutoResetEvent(false);
            messageObserver.SetEventWaitHandle(waitHandle);

            _hue_stream.RegisterFeedbackHandler(_message_handler);

            // Start listening on the ID_USERPROCEDURE_FINISHED message, connect to the bridge
            messageObserver.StartListening(FeedbackMessage.Id.ID_USERPROCEDURE_FINISHED);
            _hue_stream.ConnectManualBridgeInfoAsync(_bridge);

            // Wait for the message
            Assert.IsTrue(waitHandle.WaitOne(CONNECTION_TIMEOUT_MS), "Bridge connection timed out");
            CheckStreamConnectionValid(_hue_stream);
        }

        [Test]
        public void TestBridgeConnectivity_Connect_InvalidUsername_Async()
        {
            _bridge.SetUser("Invalid Username");

            // Create message observer, connect event handle to it and attach it to the stream
            var messageObserver = new FeedbackMessageObserver();
            _message_handler = messageObserver;

            var waitHandle = new AutoResetEvent(false);
            messageObserver.SetEventWaitHandle(waitHandle);
            _hue_stream.RegisterFeedbackHandler(messageObserver);

            // Start listening on the ID_FINISH_RETRIEVING_FAILED message, connect to the bridge
            messageObserver.StartListening(FeedbackMessage.Id.ID_FINISH_RETRIEVING_FAILED);
            _hue_stream.ConnectManualBridgeInfoAsync(_bridge);

            // Wait for the message
            Assert.IsTrue(waitHandle.WaitOne(CONNECTION_TIMEOUT_MS), "Waiting for message timed out");
        }

        //[Test]
        public void TestBridgeConnectivity_Connect_Shutdown_Reconnect()
        {
            // Connect with the fresh config
            _hue_stream.ConnectManualBridgeInfo(_bridge);
            CheckStreamConnectionValid(_hue_stream);

            // Stop streaming
            ResetStream();
            CheckStreamNotConnected(_hue_stream);

            // Try to reconnect with the same config
            _hue_stream.ConnectBridge();
            CheckStreamConnectionValid(_hue_stream);
        }

        [Test]
        public void TestBridgeConnectivity_Connect_NoEntertainmentGroups_AddGroup_Connect()
        {
            // Remove all entertainment groups on bridge and try to connect
            _bridgeWrapperHelper.RemoveEntertainmentGroups();
            Assert.IsFalse(_hue_stream.GetActiveBridge().IsGroupSelected(), "There is still selected group on bridge");
            _hue_stream.ConnectManualBridgeInfo(_bridge);
            Assert.AreEqual(_hue_stream.GetConnectionResult(), ConnectResult.ActionRequired, "Stream should expect action");

            // Stop streaming.
            ResetStream();
            CheckStreamNotConnected(_hue_stream);

            // Add new entertainment group and select it
            int entertainmentGroup = _bridgeWrapperHelper.GetEntertainmentGroupId();
            _bridge.SelectGroup(entertainmentGroup.ToString());

            // Try to connect again
            _hue_stream.ConnectManualBridgeInfo(_bridge);
            CheckStreamConnectionValid(_hue_stream);
        }
    }
}
