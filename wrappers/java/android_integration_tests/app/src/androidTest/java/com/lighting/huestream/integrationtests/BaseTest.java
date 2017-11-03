package com.lighting.huestream.integrationtests;


import android.util.Log;

import com.lighting.huestream.Bridge;
import com.lighting.huestream.BridgeSettings;
import com.lighting.huestream.Config;
import com.lighting.huestream.HueStream;
import com.lighting.huestream.StreamSettings;
import com.lighting.huestream.StreamingMode;
import com.lighting.huestream.integrationtests.helpers.BridgeWrapperBuilder;
import com.lighting.huestream.integrationtests.helpers.IBridgeWrapper;
import com.lighting.huestream.integrationtests.helpers.Network;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.junit.Assert;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.InvalidPropertiesFormatException;
import java.util.Map;

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
        result.SetTcpPort(_tcp_port.toString());

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

    protected void threadWaitFor(int milliseconds) {
        try {
            Thread.sleep(milliseconds);
        } catch (InterruptedException e) {
            Assert.fail("Thread was interrupted when sleeping");
        }
    }

    private void readBridgePropertiesIfNeeded() {
        if (_ipv4_address.isEmpty() || _bridge_id.isEmpty() || _tcp_port.isEmpty() || _udp_port.isEmpty()) {
            final String FILE_NAME = "sdcard/GRADLE_OPTS.cfg";

            try {
                final String fileContents = readFile(FILE_NAME);

                String[] paramsArray = fileContents.split(" ");
                Map<String, String> params = new HashMap<>();
                for (String paramString : paramsArray) {
                    if (!paramString.isEmpty()) {
                        paramString = paramString.trim();
                        String[] paramsPair = paramString.split("=");

                        Assert.assertEquals(paramsPair.length, 2);
                        params.put(paramsPair[0], paramsPair[1]);
                    }
                }

                _udp_port = params.get("-Dhue_streaming_port");
                if (_udp_port == null) {
                    _udp_port = DEFAULT_UDP_PORT;
                }

                _bridge_id = params.get("-Dhue_bridge_id");
                if (_bridge_id == null) {
                    _bridge_id = DEFAULT_BRIDGE_ID;
                }

                String bridge_ip = params.get("-Dhue_bridge_ip");
                if (bridge_ip == null) {
                    bridge_ip = DEFAULT_ADDRESS;
                }

                parseApplyAddressAndPort(bridge_ip);

            } catch (Exception e) {
                System.out.println("Exception, setting default params " + e);

                _ipv4_address = DEFAULT_IP_ADDRESS;
                _tcp_port = DEFAULT_TCP_PORT;
                _bridge_id = DEFAULT_BRIDGE_ID;
                _udp_port = DEFAULT_UDP_PORT;
            }

            System.out.println(String.format("IP_address=%s, tcp_port=%s, udp_port=%s, bridge_id=%s",
                    _ipv4_address, _tcp_port, _udp_port, _bridge_id));

        }
    }

    private void parseApplyAddressAndPort(final String address) throws InvalidPropertiesFormatException {
        final String[] parts = address.split(":");
        if (parts.length == 1) {
            _ipv4_address = address;
            _tcp_port = "80";
        } else if (parts.length == 2) {
            _ipv4_address = parts[0];
            _tcp_port = parts[1];
        } else {
            throw new InvalidPropertiesFormatException("can not parse address: " + address);
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

    private void createUserIfNeeded() {
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

            try {
                JSONObject responseRoot = response.getJSONObject(0);
                JSONObject successNode = responseRoot.getJSONObject("success");

                _user = successNode.getString("username");
                _clientKey = successNode.getString("clientkey");

                Assert.assertFalse("User name is empty", _user.isEmpty());
                Assert.assertFalse("Client key is empty", _clientKey.isEmpty());
            } catch (JSONException e) {
                Assert.fail("JSONException caught");
            }
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

        try {
            JSONObject responseRoot = response.getJSONObject(0);
            responseRoot.get("success");
        } catch (JSONException e) {
            Assert.fail("JSONException caught");
        }
    }

    private String _user = "";
    private String _clientKey = "";
    private static String _ipv4_address = "";
    private static String _bridge_id = "";
    private static String _tcp_port = "";
    private static String _udp_port = "";

        private static String DEFAULT_IP_ADDRESS = "192.168.1.51";
    private static String DEFAULT_TCP_PORT = "60202";
    private static String DEFAULT_UDP_PORT = "60202";
    private static String DEFAULT_ADDRESS = DEFAULT_IP_ADDRESS + ":" + DEFAULT_TCP_PORT;
    private static String DEFAULT_BRIDGE_ID = "001788fffe1ffd08";

}
