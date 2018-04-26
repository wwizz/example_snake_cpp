/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/connect/BridgeFileStorageAccessor.h>

#include <fstream>
#include <sstream>
#include <string>
#include <memory>

namespace huestream {

    BridgeFileStorageAccessor::BridgeFileStorageAccessor(const std::string &fileName, BridgeSettingsPtr bridgeSettings) :
            _fileName(fileName),
            _bridgeSettings(bridgeSettings) {
    }

    void BridgeFileStorageAccessor::Load(BridgesLoadCallbackHandler cb) {
        std::fstream file;
        file.open(_fileName.c_str(), std::fstream::in);
        if (file.is_open()) {
            std::ostringstream contents;
            contents << file.rdbuf();
            file.close();

            auto bridgeList = std::make_shared<HueStreamData>(_bridgeSettings);
            bridgeList->DeserializeText(contents.str());

            cb(OPERATION_SUCCESS, bridgeList);
        } else {
            cb(OPERATION_FAILED, std::make_shared<HueStreamData>(_bridgeSettings));
        }
    }

    void BridgeFileStorageAccessor::Save(HueStreamDataPtr bridges, BridgesSaveCallbackHandler cb) {
        std::ofstream file;
        file.open(_fileName.c_str(), std::fstream::out | std::fstream::trunc);
        if (file.is_open()) {
            auto jc = bridges->SerializeText();
            std::ostringstream contents(jc);
            file << contents.str();
            file.close();
            cb(OPERATION_SUCCESS);
        } else {
            cb(OPERATION_FAILED);
        }
    }
}  // namespace huestream
