/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/stream/StreamStarter.h>
#include <libjson/libjson.h>
#include <huestream/config/Config.h>
#include <huestream/common/http/HttpClientProvider.h>

#include <sstream>
#include <string>
#include <memory>

namespace huestream {

#define CLIP_ERROR_TYPE_UNAUTHORIZED_USER 1
#define CLIP_ERROR_TYPE_RESOURCE_NOT_AVAILABLE 3
#define CLIP_ERROR_TYPE_PARAMETER_NOT_AVAILABLE 6
#define CLIP_ERROR_TYPE_CANT_ACTIVATE 307

std::string SerializeJson(const JSONNode &j) {
        return j.write();
    }

    StreamStarter::StreamStarter(BridgePtr bridge, HttpClientPtr http) : _bridge(bridge), _http(http) {}

    StreamStarter::~StreamStarter() {}

    bool StreamStarter::Start(bool force) {
        auto result = Execute(true);
        if (!result && _bridge->IsBusy() && force) {
            Execute(false);
            result = Execute(true);
        }
        if (result) {
            _bridge->GetGroup()->SetActive(true);
            _bridge->GetGroup()->SetOwner(_bridge->GetUser());
        } else if (_bridge->IsBusy() && _bridge->IsValidGroupSelected() && force) {
            _bridge->GetGroup()->SetActive(false);
        }
        return result;
    }

    void StreamStarter::Stop() {
        if (Execute(false)) {
            _bridge->GetGroup()->SetActive(false);
        }
    }

    bool StreamStarter::Execute(bool activate) {
        auto url = _bridge->GetSelectedGroupUrl();

        JSONNode streamNode;
        streamNode.set_name("stream");
        streamNode.push_back(JSONNode("active", activate));

        JSONNode groupNode;
        groupNode.push_back(streamNode);
        auto data = SerializeJson(groupNode);
        auto req = std::make_shared<HttpRequestInfo>(HTTP_REQUEST_PUT, url, data);
        _http->Execute(req);

        return CheckForErrors(req);
    }

    bool StreamStarter::CheckForErrors(HttpRequestPtr req) {
        if (!req->GetSuccess()) {
            _bridge->SetIsValidIp(false);
            return false;
        }

        JSONNode node = libjson::parse(req->GetResponse());
        if (node.type() != JSON_ARRAY) {
            return false;
        }

        for (auto it = node.begin(); it != node.end(); ++it) {
            auto entry = it->find("error");
            if (entry != it->end()) {
                auto i = entry->find("type");
                if (i != entry->end()) {
                    if (i->as_int() == CLIP_ERROR_TYPE_UNAUTHORIZED_USER) {
                        _bridge->SetIsAuthorized(false);
                    }
                    if (IsNotExistingOrInvalidGroup(static_cast<int>(i->as_int()))) {
                        _bridge->DeleteGroup(_bridge->GetSelectedGroup());
                    }
                    if (i->as_int() == CLIP_ERROR_TYPE_CANT_ACTIVATE) {
                        _bridge->SetIsBusy(true);
                    }
                }
                return false;
            }
        }

        _bridge->SetIsBusy(false);

        return true;
    }

    bool StreamStarter::IsNotExistingOrInvalidGroup(int errorCode) {
        return (errorCode == CLIP_ERROR_TYPE_RESOURCE_NOT_AVAILABLE ||
                errorCode == CLIP_ERROR_TYPE_PARAMETER_NOT_AVAILABLE);
    }

}  // namespace huestream
