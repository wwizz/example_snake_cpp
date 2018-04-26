/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
/** @file */

#ifndef HUESTREAM_COMMON_DATA_GROUP_H_
#define HUESTREAM_COMMON_DATA_GROUP_H_

#include <huestream/common/data/Light.h>
#include <huestream/common/data/Scene.h>

#include <map>
#include <vector>
#include <memory>
#include <string>

namespace huestream {

    /**
     class of entertainment group, indicating by what reference it has been created
     */
    enum GroupClass {
        GROUPCLASS_TV,      ///< Screen is used as reference point (e.g. for video)
        GROUPCLASS_FREE,    ///< No reference point is used  (e.g. for audio)
        GROUPCLASS_OTHER    ///< Unknown (application defined) reference point
    };

    /**
     node which is the proxy for sending stream messages in the mesh network
     */
    typedef struct {
        std::string uri;
        std::string mode;
        std::string name;
        std::string model;
        bool isReachable;
    } GroupProxyNode;

    /**
     defintion of a group of lights used as an entertainment setup to play light effects on
     */
    class Group : public Serializable {
    public:
        static constexpr const char* type = "huestream.Group";

    /**
     Set identifier of this group
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE(Group, std::string, id, Id);

    /**
     Set user given name of this group
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE(Group, std::string, name, Name);

    /**
     Set class type of this group
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE(Group, GroupClass, classType, ClassType);

    /**
     Set list of lights in this group
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE(Group, LightListPtr, lights, Lights);

    /**
     Set whether this group is currently being streamed to
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE_BOOL(Group, bool, active, Active);

    /**
     Set id of application currently streaming to this group
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE(Group, std::string, owner, Owner);

    /**
     Set name of application currently streaming to this group
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE(Group, std::string, ownerName, OwnerName);

    /**
     Set proxy node
     @note mostly internal use, imported from bridge
     */
    PROP_DEFINE(Group, GroupProxyNode, proxyNode, ProxyNode);

    /**
     Get list of scenes for this group
     */
    PROP_DEFINE(Group, SceneListPtr, scenes, Scenes);

    /**
     Get on state for this group
     @note only relevant when not streaming
     */
    PROP_DEFINE_BOOL(Group, bool, onState, OnState);

    /**
     Get brightness state for this group
     @note only relevant when not streaming
     */
    PROP_DEFINE(Group, double, brightnessState, BrightnessState);

    public:
        /**
         constructor
         */
        Group();

        /**
         destructor
         */
        virtual ~Group();

        /**
         add a light to this group
         */
        void AddLight(std::string id, double x, double y, std::string name = "", std::string model = "", bool reachable = true);

        /**
         get readable version of the owner name
         */
        std::string GetFriendlyOwnerName() const;

        /**
         get owner application name
         */
        std::string GetOwnerApplicationName() const;

        /**
         get owner device name (if available, else empty string)
         */
        std::string GetOwnerDeviceName() const;

        void Serialize(JSONNode *node) const override;

        void Deserialize(JSONNode *node) override;

        std::string GetTypeName() const override;

        Group* Clone();

    private:
        double Clip(double value, double min, double max) const;
        void SerializeClass(JSONNode *node) const;
        void DeserializeClass(JSONNode *node);
        std::vector<std::string> Split(const std::string& s, char delimiter) const;

        static const std::map<GroupClass, std::string> _classSerializeMap;
    };

    /**
     shared pointer to a huestream::Group object
     */
    SMART_POINTER_TYPES_FOR(Group)
}  // namespace huestream

#endif  // HUESTREAM_COMMON_DATA_GROUP_H_
