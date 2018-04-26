package com.lighting.huestream.tests.integration.helpers;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.*;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.logging.Logger;

public class Network {
    public enum UPDATE_REQUEST {
        POST,
        PUT
    }

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

            BufferedReader reader = new BufferedReader(new InputStreamReader(httpConnection.getInputStream()));
            JSONParser parser = new JSONParser();
            return (JSONArray) parser.parse(reader);
        } catch (MalformedURLException e) {
            Logger.getGlobal().warning("Malformed url, when doing the request: " + e);
        } catch (IOException e) {
            Logger.getGlobal().warning("IOException when opening url connection: " + e);
        } catch (ParseException e) {
            Logger.getGlobal().warning("Can not parse incoming JSON: " + e);
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

            BufferedReader reader = new BufferedReader(new InputStreamReader(httpConnection.getInputStream()));
            JSONParser parser = new JSONParser();
            JSONArray result = (JSONArray) parser.parse(reader);

            return result;
        } catch (MalformedURLException e) {
            Logger.getGlobal().warning("Malformed url, when doing the request: " + e);
        } catch (IOException e) {
            Logger.getGlobal().warning("IOException when opening url connection: " + e);
        } catch (ParseException e) {
            Logger.getGlobal().warning("Can not parse incoming JSON: " + e);
        }

        return null;
    }

    public static JSONObject performGetRequest(final String fullUrl) {
        try {
            URL url = new URL(fullUrl);
            HttpURLConnection httpConnection = (HttpURLConnection) url.openConnection();
            httpConnection.setRequestMethod("GET");

            BufferedReader reader = new BufferedReader(new InputStreamReader(httpConnection.getInputStream()));
            JSONParser parser = new JSONParser();
            JSONObject result = (JSONObject) parser.parse(reader);

            return result;
        } catch (MalformedURLException e) {
            Logger.getGlobal().warning("Malformed url, when doing the request: " + e);
        } catch (IOException e) {
            Logger.getGlobal().warning("IOException when opening url connection: " + e);
        } catch (ParseException e) {
            Logger.getGlobal().warning("Can not parse incoming JSON: " + e);
        }

        return null;
    }
}
