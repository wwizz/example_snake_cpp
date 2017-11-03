/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_CONNECT_BRIDGEFILESTORAGEACCESSOR_H_
#define HUESTREAM_CONNECT_BRIDGEFILESTORAGEACCESSOR_H_

#include <huestream/connect/IBridgeStorageAccessor.h>

#include <string>

namespace huestream {

    class BridgeFileStorageAccessor : public IBridgeStorageAccessor {
    public:
        BridgeFileStorageAccessor(const std::string &fileName, BridgeSettingsPtr hueSettings);

        void Load(BridgesLoadCallbackHandler cb) override;

        void Save(HueStreamDataPtr bridges, BridgesSaveCallbackHandler cb) override;

    private:
        std::string _fileName;
        BridgeSettingsPtr _bridgeSettings;
    };

}  // namespace huestream

#endif  // HUESTREAM_CONNECT_BRIDGEFILESTORAGEACCESSOR_H_
