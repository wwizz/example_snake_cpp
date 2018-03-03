package com.lighting.huestream.tests.integration;

import com.lighting.huestream.*;
import com.lighting.huestream.tests.integration.helpers.BridgeWrapperBuilder;
import com.lighting.huestream.tests.integration.helpers.IBridgeWrapper;
import com.lighting.huestream.tests.integration.helpers.Network;
import org.json.simple.JSONObject;
import org.junit.Assert;
import org.json.simple.JSONArray;

import java.io.*;
import java.text.ParseException;
import java.util.Arrays;
import java.util.InvalidPropertiesFormatException;
import java.util.Map;
import java.util.stream.Collectors;

public class BaseTest {


    protected static int DEFAULT_TIMEOUT_MS = 3000;

    protected Bridge _bridge = null;
    protected HueStream _hue_stream = null;
    protected IBridgeWrapper _bridgeWrapperHelper = null;

    protected Bridge initializeBridge() {
        readBridgePropertiesIfNeeded();
        createUserIfNeeded();

        Bridge result = new Bridge(_bridge_id, _ipv4_address, true, new BridgeSettings());

        result.SetUser(_user);
        result.SetClientKey(_clientKey);
        result.SetTcpPort(_tcp_port);
        result.SetSslPort(_ssl_port);
        result.EnableSsl();

        return result;
    }

    protected IBridgeWrapper initializeBridgeWrapper()
    {
        readBridgePropertiesIfNeeded();
        createUserIfNeeded();

        BridgeWrapperBuilder builder = new BridgeWrapperBuilder();
        builder.withUserName(_user)
                .withClientKey(_clientKey)
                .withIPv4Address(_ipv4_address)
                .withTcpPort(Integer.parseInt(_tcp_port))
                .withBridgeId(_bridge_id);


        return builder.build();
    }

    protected HueStream createStream(StreamingMode streamingMode) {
        readBridgePropertiesIfNeeded();

        StreamSettings streamSettings = new StreamSettings();
        streamSettings.SetStreamingPort(Integer.parseInt(_udp_port));

        Config config = new Config("JavaIntegrationTests", "PC");
        config.SetStreamSettings(streamSettings);
        config.SetStreamingMode(streamingMode);
        return new HueStream(config);
    }

    private void readBridgePropertiesIfNeeded() {
        if (_ipv4_address.isEmpty() || _bridge_id.isEmpty() || _tcp_port.isEmpty() || _udp_port.isEmpty()) {
            final String FILE_NAME = "ANT_OPTS.cfg";

            try {
                final String fileContents = readFile(FILE_NAME);

                // attempting to parse and verify the file contents
                Map<String, String> params = Arrays.stream(fileContents.split(" "))
                        .filter(token -> !token.isEmpty())
                        .map(token -> token.replaceAll("[\"']", "")) // removing the quotes - for Windows
                        .map(token -> token.split("="))
                        .collect(Collectors.toMap(
                                pair -> pair[0], //key
                                pair -> pair[1]  // value
                        ));

                _udp_port = params.getOrDefault("-Dhue_streaming_port", DEFAULT_UDP_PORT);
                _bridge_id = params.getOrDefault("-Dhue_bridge_id", DEFAULT_BRIDGE_ID);
                _ssl_port = params.getOrDefault("-Dhue_https_port", DEFAULT_SSL_PORT);
                _tcp_port = params.getOrDefault("-Dhue_http_port", DEFAULT_TCP_PORT);
                _ipv4_address = params.getOrDefault("-Dhue_ip", DEFAULT_IP_ADDRESS);
            } catch (Exception e) {
                System.out.println("Exception, setting default params " + e);

                _ipv4_address = DEFAULT_IP_ADDRESS;
                _tcp_port = DEFAULT_TCP_PORT;
                _bridge_id = DEFAULT_BRIDGE_ID;
                _udp_port = DEFAULT_UDP_PORT;
                _ssl_port = DEFAULT_SSL_PORT;
            }

            System.out.println(String.format("IP_address=%s, tcp_port=%s, udp_port=%s, ssl_port=%s, bridge_id=%s",
                    _ipv4_address, _tcp_port, _udp_port, _ssl_port, _bridge_id));

        }
    }

    private String readFile(final String fileName) throws IOException {
        InputStream fis = new FileInputStream(fileName);
        InputStreamReader isr = new InputStreamReader(fis);
        BufferedReader reader = new BufferedReader(isr);

        StringBuilder fileContents = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            fileContents.append(line);
        }

        return fileContents.toString();
    }

    private void createUserIfNeeded()  {
        if (_user.isEmpty() || _clientKey.isEmpty()) {
            pushLink();

            final StringBuilder urlBuilder = new StringBuilder();
            urlBuilder.append("http://")
                    .append(_ipv4_address)
                    .append(":")
                    .append(_tcp_port)
                    .append("/api");

            final String REQUEST_BODY = "{\"devicetype\": \"JavaIntegrationTests#PC\", \"generateclientkey\":true}";

            JSONArray response = Network.performUpdateRequest(urlBuilder.toString(), REQUEST_BODY, Network.UPDATE_REQUEST.POST);
            Assert.assertNotNull("Push link response is null", response);

            JSONObject responseRoot = (JSONObject) response.iterator().next();
            Assert.assertNotNull("Wrong structure of incoming response", responseRoot);

            JSONObject successNode = (JSONObject) responseRoot.get("success");
            Assert.assertNotNull("Push link was not successfull", successNode);

            _user = (String) successNode.get("username");
            Assert.assertNotNull("User name is null", _user);
            Assert.assertFalse("User name is empty", _user.isEmpty());

            _clientKey = (String) successNode.get("clientkey");
            Assert.assertNotNull("Client key is null", _clientKey);
            Assert.assertFalse("Client key is empty", _clientKey.isEmpty());
        }
    }

    private void pushLink() {
        final StringBuilder urlBuilder = new StringBuilder();
        urlBuilder.append("http://")
                .append(_ipv4_address)
                .append(":")
                .append(_tcp_port)
                .append("/api/0/config");

        final String REQUEST_BODY = "{\"linkbutton\":true}";

        JSONArray response = Network.performUpdateRequest(urlBuilder.toString(), REQUEST_BODY, Network.UPDATE_REQUEST.PUT);
        Assert.assertNotNull("Push link response is null", response);

        JSONObject responseRoot = (JSONObject) response.iterator().next();
        Assert.assertNotNull("Wrong structure of incoming response", responseRoot);

        Object successNode = responseRoot.get("success");
        Assert.assertNotNull("Push link was not successfull", successNode);
    }


    private String _user = "";
    private String _clientKey = "";
    private String _ipv4_address = "";
    private String _bridge_id = "";
    private String _tcp_port = "";
    private String _udp_port = "";
    private String _ssl_port = "";


    private static String DEFAULT_IP_ADDRESS = "192.168.1.51";
    private static String DEFAULT_TCP_PORT = "60202";
    private static String DEFAULT_UDP_PORT = "60202";
    private static String DEFAULT_SSL_PORT = "61202";
    private static String DEFAULT_BRIDGE_ID = "001788fffe1ffd08";

}
