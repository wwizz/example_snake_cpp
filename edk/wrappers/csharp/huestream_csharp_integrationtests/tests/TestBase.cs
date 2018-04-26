using System.Collections.Generic;
using NUnit.Framework;
using huestream;
using System.Linq;
using System;
using System.Text;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;

namespace huestream_tests
{
    public class TestBase
    {
        protected const String APPLICATION_NAME = "CSharpIntegrationTests";

        protected String IPv4_ADDRESS = "192.168.1.51";
        protected String BRIDGE_ID = "001788fffe1ffd08";
        protected int TCP_PORT = 60202;
        protected String UDP_PORT = "60202";
        protected String SSL_PORT = "61202";
        protected const int CONNECTION_TIMEOUT_MS = 3000;

        protected FeedbackMessageHandler _message_handler;
        protected HueStream _hue_stream;
        protected Bridge _bridge;
        protected IBridgeWrapper _bridgeWrapperHelper;

        public TestBase()
        {
            if (TestContext.Parameters.Count > 0)
            {
                IPv4_ADDRESS = TestContext.Parameters["hue_ip"];
                BRIDGE_ID = TestContext.Parameters["hue_bridge_id"];
                UDP_PORT = TestContext.Parameters["hue_streaming_port"];
                SSL_PORT = TestContext.Parameters["hue_https_port"];
                TCP_PORT = int.Parse(TestContext.Parameters["hue_http_port"]);
            }
        }

        public Bridge InitializeBridge()
        {
            if (GetUser().Length == 0 || GetClientKey().Length == 0)
            {
                CreateUser();
            }

            Bridge result = new Bridge(BRIDGE_ID, IPv4_ADDRESS, true, new BridgeSettings());

            result.SetUser(GetUser());
            result.SetClientKey(GetClientKey());
            result.SetTcpPort(TCP_PORT.ToString());
            result.SetSslPort(SSL_PORT);

            return result;
        }

        public IBridgeWrapper InitializeBridgeWrapper()
        {
            if (GetUser().Length == 0 || GetClientKey().Length == 0)
            {
                CreateUser();
            }

            BridgeWrapperBuilder builder = new BridgeWrapperBuilder();
            builder.WithUserName(GetUser())
                   .WithClientKey(GetClientKey())
                   .WithIPv4Address(IPv4_ADDRESS)
                   .WithTcpPort(TCP_PORT)
                   .WithBridgeId(BRIDGE_ID);

            return builder.Build();
        }

        protected HueStream CreateStream(StreamingMode streamingMode)
        {
            StreamSettings streamSettings = new StreamSettings();
            streamSettings.SetStreamingPort(int.Parse(UDP_PORT));

            Config config = new Config(APPLICATION_NAME, Environment.OSVersion.Platform.ToString());
            config.SetStreamSettings(streamSettings);
            config.SetStreamingMode(streamingMode);
            return new HueStream(config);
        }

        protected void CleanupUser()
        {
            _user = "";
            _clientKey = "";
            _bridgeWrapperHelper.CleanUpUser();
        }

        private String GetUser()
        {
            return _user;
        }

        private String GetClientKey()
        {
            return _clientKey;
        }
        private void CreateUser()
        {
            PushLink();

            StringBuilder urlBuilder = new StringBuilder();
            urlBuilder.Append("http://")
                    .Append(IPv4_ADDRESS)
                    .Append(":")
                    .Append(TCP_PORT)
                    .Append("/api");

            String REQUEST_BODY = "{\"devicetype\": \"" + APPLICATION_NAME + "\", \"generateclientkey\":true}";

            JArray response = Network.PerformUpdateRequest(urlBuilder.ToString(), REQUEST_BODY, Network.UPDATE_REQUEST.POST);
            Assert.NotNull(response, "Push link response is null");

            JToken responseRoot = response[0];
            Assert.NotNull(responseRoot, "Wrong structure of incoming response");

            var successNode = responseRoot["success"];
            Assert.NotNull(successNode, "Creating group was not successful");

            _user = (string)successNode["username"];
            Assert.NotNull(_user, "User name is null");
            Assert.False(_user.Length == 0, "User name is empty");

            _clientKey = (string)successNode["clientkey"];
            Assert.NotNull(_clientKey, "Client key is null");
            Assert.False(_clientKey.Length == 0, "Client key is empty");
        }

        private void PushLink()
        {
            StringBuilder urlBuilder = new StringBuilder();
            urlBuilder.Append("http://")
                    .Append(IPv4_ADDRESS)
                    .Append(":")
                    .Append(TCP_PORT)
                    .Append("/api/0/config");

            String REQUEST_BODY = "{\"linkbutton\":true}";

            JArray response = Network.PerformUpdateRequest(urlBuilder.ToString(), REQUEST_BODY, Network.UPDATE_REQUEST.PUT);
            Assert.NotNull(response, "Push link response is null");

            JToken responseRoot = response[0];
            Assert.NotNull(responseRoot, "Wrong structure of incoming response");

            var successNode = responseRoot["success"];
            Assert.NotNull(successNode, "Push link was not successful");
        }


        private String _user = "";
        private String _clientKey = "";
    }
}
