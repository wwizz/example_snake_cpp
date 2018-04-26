/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_STREAM_PROTOCOLSERIALIZER_H_
#define HUESTREAM_STREAM_PROTOCOLSERIALIZER_H_

#include <huestream/common/data/Group.h>

#include <memory>
#include <vector>

namespace huestream {

    typedef enum {
        COLORSPACE_RGB = 0x00,
        COLORSPACE_XYBRI = 0x01
    } ColorSpace;

    typedef struct {
        ColorSpace colorSpace;
        std::shared_ptr<Group> group;
    } StreamOptions;

    class ProtocolSerializer {
    public:
        explicit ProtocolSerializer(std::shared_ptr<StreamOptions> options);

        virtual ~ProtocolSerializer();

        std::vector<uint8_t> Serialize(uint8_t seqNr);

    protected:
        std::shared_ptr<StreamOptions> _options;
    };
}  // namespace huestream

#endif  // HUESTREAM_STREAM_PROTOCOLSERIALIZER_H_
