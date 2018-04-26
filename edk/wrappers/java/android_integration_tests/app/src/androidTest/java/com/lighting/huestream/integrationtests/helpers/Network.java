package com.lighting.huestream.integrationtests.helpers;

import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.logging.Logger;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.*;

public class Network {
    public enum UPDATE_REQUEST {
        POST,
        PUT
    }

    private static final String TAG = Network.class.getName();

    public static JSONArray performUpdateRequest(final String fullUrl, final String serializedRequest, final UPDATE_REQUEST type) {
        try {
            URL url = new URL(fullUrl);
            HttpURLConnection httpConnection = (HttpURLConnection) url.openConnection();
            httpConnection.setRequestMethod(type.name());
            httpConnection.setRequestProperty("Content-Length", String.valueOf(serializedRequest.length()));
            httpConnection.setDoOutput(true);
            httpConnection.connect();

            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(httpConnection.getOutputStream()));
            writer.write(serializedRequest);
            writer.flush();
            writer.close();

            final String jsonString = extractFromStream(httpConnection.getInputStream());
            return new JSONArray(jsonString);
        } catch (MalformedURLException e) {
            Log.w(TAG,"Malformed url, when doing the request: " + e);
        } catch (IOException e) {
            Log.w(TAG, "IOException when opening url connection: " + e);
        } catch (JSONException e) {
            Log.w(TAG, "Can not parse incoming JSON: " + e);
        }

        return null;
    }

    public static JSONArray performUpdateRequest(final String fullUrl, final JSONObject request, final UPDATE_REQUEST type) {
        return performUpdateRequest(fullUrl, request.toString(), type);
    }

    public static JSONArray performDeleteRequest(final String fullUrl) {
        try {
            URL url = new URL(fullUrl);
            HttpURLConnection httpConnection = (HttpURLConnection) url.openConnection();
            httpConnection.setRequestMethod("DELETE");

            final String jsonString = extractFromStream(httpConnection.getInputStream());
            return new JSONArray(jsonString);
        } catch (MalformedURLException e) {
            Log.w(TAG, "Malformed url, when doing the request: " + e);
        } catch (IOException e) {
            Log.w(TAG, "IOException when opening url connection: " + e);
        } catch (JSONException e) {
            Log.w(TAG, "Can not parse incoming JSON: " + e);
        }

        return null;
    }

    public static JSONObject performGetRequest(final String fullUrl) {
        try {
            URL url = new URL(fullUrl);
            HttpURLConnection httpConnection = (HttpURLConnection) url.openConnection();
            httpConnection.setRequestMethod("GET");

            final String jsonString = extractFromStream(httpConnection.getInputStream());
            return new JSONObject(jsonString);
        } catch (MalformedURLException e) {
            Log.w(TAG, "Malformed url, when doing the request: " + e);
        } catch (IOException e) {
            Log.w(TAG, "IOException when opening url connection: " + e);
        } catch (JSONException e) {
            Log.w(TAG, "Can not parse incoming JSON: " + e);
        }

        return null;
    }

    private static String extractFromStream(final InputStream stream) throws IOException {
        final BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
        final StringBuilder builder = new StringBuilder();

        String line;
        while ((line = reader.readLine()) != null) {
            builder.append(line);
        }

        return builder.toString();
    }
}
