using System;
using System.IO;
using System.Net;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public static class Network
{
    public enum UPDATE_REQUEST
    {
        POST,
        PUT
    }

    public static JArray PerformUpdateRequest(String fullUrl, String serializedRequest, UPDATE_REQUEST type)
    {
        var url = new Uri(fullUrl);
        byte[] requestByteArray = Encoding.UTF8.GetBytes(serializedRequest);

        var httpRequest = WebRequest.CreateHttp(url);
        httpRequest.Method = type.ToString();
        httpRequest.ContentType = "application/x-www-form-urlencoded";
        httpRequest.Accept = "application/json";
        httpRequest.ContentLength = serializedRequest.Length;

        // Write data to stream
        using (var stream = httpRequest.GetRequestStream())
        {
            stream.Write(requestByteArray, 0, requestByteArray.Length);
            stream.Close();
        }

        String jsonResponse = string.Empty;

        // Get response
        using (var response = (HttpWebResponse)httpRequest.GetResponse())
        using (var stream = response.GetResponseStream())
        using (var reader = new StreamReader(stream))
        {
            jsonResponse = reader.ReadToEnd();
        }

        return JArray.Parse(jsonResponse);
    }

    public static JArray PerformUpdateRequest(String fullUrl, JObject request, UPDATE_REQUEST type)
    {
        return PerformUpdateRequest(fullUrl, request.ToString(Formatting.None), type);
    }

    public static JArray PerformDeleteRequest(String fullUrl)
    {
        var url = new Uri(fullUrl);

        var httpRequest = WebRequest.CreateHttp(url);
        httpRequest.Method = "DELETE";
        httpRequest.Accept = "application/json";

        String jsonResponse = string.Empty;

        using (var response = (HttpWebResponse)httpRequest.GetResponse())
        using (var stream = response.GetResponseStream())
        using (var reader = new StreamReader(stream))
        {
            jsonResponse = reader.ReadToEnd();
        }

        return JArray.Parse(jsonResponse);
    }

    public static JObject PerformGetRequest(String fullUrl)
    {
        var url = new Uri(fullUrl);

        var httpRequest = WebRequest.CreateHttp(url);
        httpRequest.Method = WebRequestMethods.Http.Get;
        httpRequest.Accept = "application/json";

        String jsonResponse = string.Empty;

        using (var response = (HttpWebResponse)httpRequest.GetResponse())
        using (var stream = response.GetResponseStream())
        using (var reader = new StreamReader(stream))
        {
            jsonResponse = reader.ReadToEnd();
        }

        return JObject.Parse(jsonResponse);
    }
}