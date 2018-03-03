/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_EFFECT_ANIMATION_DATA_CURVEDATA_H_
#define HUESTREAM_EFFECT_ANIMATION_DATA_CURVEDATA_H_

#include <huestream/effect/animation/data/Point.h>
#include <huestream/effect/animation/data/CurveOptions.h>

#include <vector>
#include <memory>
#include <string>

namespace huestream {

    class CurveData : public virtual Serializable {
    public:
        static constexpr const char* type = "huestream.CurveData";

        PROP_DEFINE(CurveData, Nullable<CurveOptions>, options, Options);
        PROP_DEFINE(CurveData, PointListPtr, points, Points);

    public:
        void Serialize(JSONNode *node) const override;

        void Deserialize(JSONNode *node) override;

        std::string GetTypeName() const override;

        CurveData();

        virtual ~CurveData() {}

        explicit CurveData(PointListPtr points, Nullable<CurveOptions> options = Nullable<CurveOptions>());

        PointPtr GetInterpolated(double x) const;
        double GetStepValue(double x) const;

        double GetLength() const;

        double GetBegin() const;

        double GetEnd() const;

        bool HasPoints() const;

        const CurveData &Append(const CurveData &other) const;

    private:
        void CorrectPoint(PointPtr p) const;

        bool GetStartAndEndPoint(double x, PointPtr* start, PointPtr* end) const;

        bool GetStartPointIndex(double position, unsigned int* index) const;
    };
}  // namespace huestream

#endif  // HUESTREAM_EFFECT_ANIMATION_DATA_CURVEDATA_H_
