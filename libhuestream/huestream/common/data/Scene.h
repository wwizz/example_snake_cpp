/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
/** @file */

#ifndef HUESTREAM_COMMON_DATA_SCENE_H_
#define HUESTREAM_COMMON_DATA_SCENE_H_

#include <huestream/common/serialize/SerializerHelper.h>
#include <huestream/common/serialize/Serializable.h>

#include <memory>
#include <vector>
#include <string>


namespace huestream {

    /**
     defintion of a light scene
     */
    class Scene : public Serializable {
    public:
        static constexpr const char* type = "huestream.Scene";

    /**
     Get tag of scene
     */
    PROP_DEFINE(Scene, std::string, tag, Tag);

    /**
     Get name of scene
     */
    PROP_DEFINE(Scene, std::string, name, Name);

    /**
     Get tag of scene
     */
    PROP_DEFINE(Scene, std::string, appData, AppData);

    
    public:
        Scene();

        /**
         constructor
         @param tag Scene tag
         @param name Scene friendly name
         @param appData Scene app data field
         */
        explicit Scene(std::string tag, std::string name = "", std::string appData = "");

        /**
         destructor
         */
        virtual ~Scene();

        std::string GetGroupId() const;

        int GetDefaultSceneId() const;

        void Serialize(JSONNode *node) const override;

        void Deserialize(JSONNode *node) override;

        std::string GetTypeName() const override;
    };

    /**
     shared pointer to a huestream::Scene object
     */
    SMART_POINTER_TYPES_FOR(Scene);

}  // namespace huestream

#endif  // HUESTREAM_COMMON_DATA_SCENE_H_
