/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/storage/FileStorageAccessor.h>

#include <fstream>
#include <sstream>
#include <string>

namespace huestream {

    FileStorageAccessor::FileStorageAccessor(const std::string &fileName) : _fileName(fileName) {
    }

    void FileStorageAccessor::Load(LoadCallbackHandler cb) {
        std::fstream file;
        file.open(_fileName.c_str(), std::fstream::in);
        if (file.is_open()) {
            std::ostringstream contents;
            contents << file.rdbuf();
            file.close();

            JSONNode node = libjson::parse(contents.str());
            auto s = Serializable::DeserializeFromJson(&node);
            cb(OPERATION_SUCCESS, s);
        } else {
            cb(OPERATION_FAILED, nullptr);
        }
    }

    void FileStorageAccessor::Save(SerializablePtr serializable, SaveCallbackHandler cb) {
        JSONNode node;
        serializable->Serialize(&node);
        std::ofstream file;
        file.open(_fileName.c_str(), std::fstream::out | std::fstream::trunc);
        if (file.is_open()) {
            std::string jc = node.write_formatted();
            std::ostringstream contents(jc);
            file << contents.str();
            file.close();
            cb(OPERATION_SUCCESS);
        } else {
            cb(OPERATION_FAILED);
        }
    }
}  // namespace huestream
