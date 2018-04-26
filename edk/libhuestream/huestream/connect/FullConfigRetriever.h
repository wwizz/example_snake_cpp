/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_FULLCONFIGRETRIEVER_H_
#define HUESTREAM_CONNECT_FULLCONFIGRETRIEVER_H_

#include <memory>
#include <string>

#include "huestream/connect/IFullConfigRetriever.h"
#include "huestream/common/data/Bridge.h"
#include "libjson/libjson.h"
#include "huestream/common/http/IHttpClient.h"
#include "support/network/http/HttpRequest.h"


namespace huestream {
    class LightInfo : public Light {
    public:
        double GetBrightness() const { return _brightness; }
        void SetBrightness(double brightness) { _brightness = brightness; }
    private:
        double _brightness = 0.0;
    };

    class ConfigRetriever : public IConfigRetriever {
    public:
        explicit ConfigRetriever(const HttpClientPtr http, bool useForcedActivation = true, ConfigType configType = ConfigType::Full);

        bool Execute(BridgePtr bridge, RetrieveCallbackHandler cb) override;

    protected:
        ConfigType _configType;
        HttpClientPtr _http;
        bool _useForcedActivation;
        std::mutex _mutex;
        bool _busy;
        shared_ptr<support::HttpRequestBase> _request;
        std::string _response;
        BridgePtr _bridge;
        RetrieveCallbackHandler _cb;
        JSONNode _root;

        void RetrieveConfig();

        void ParseResponseAndExecuteCallback();

        bool ParseToJson();

        bool CheckForNoErrorsInResponse() const;

        bool ParseConfig() const;

        void ParseGroups();

        GroupPtr ParseEntertainmentGroup(const JSONNode &node);

        bool GroupIsEntertainment(const JSONNode &j);

        void ParseClass(const JSONNode &node, GroupPtr group);

        void ParseLightsAndLocations(const JSONNode &node, GroupPtr group) const;

        void ParseStream(const JSONNode &node, GroupPtr group);

        void ParseStreamActive(const JSONNode &node, GroupPtr group);

        void ParseStreamProxy(const JSONNode &node, GroupPtr group);

        void ParseGroupState(const JSONNode &node, GroupPtr group);

        std::string GetOwnerName(const std::string &userName);

        LightInfo GetLightInfo(const std::string &id) const;

        void ParseScenes(const JSONNode &node, GroupPtr group);

        bool SceneIsNotRecyclable(const JSONNode &j);

        ScenePtr ParseScene(const JSONNode &node);

        void ParseCapabilities() const;

        void Finish(OperationResult result);
    };

    using FullConfigRetriever = ConfigRetriever;
}  // namespace huestream

#endif  // HUESTREAM_CONNECT_FULLCONFIGRETRIEVER_H_
