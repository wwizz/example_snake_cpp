/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_EFFECT_ANIMATION_DATA_VECTOR_H_
#define HUESTREAM_EFFECT_ANIMATION_DATA_VECTOR_H_

#include <huestream/effect/animation/data/Point.h>
#include <huestream/common/data/Location.h>

namespace huestream {

    class Vector {
    private:
        double _x;
        double _y;
    public:
        Vector(const Point &from, const Point &to);

        Vector(const Location &from, const Location &to);

        const double &get_x() const;

        const double &get_y() const;


        int GetQuadrant() const;

        double GetLength() const;

        double GetAngle() const;
    };
}  // namespace huestream

#endif  // HUESTREAM_EFFECT_ANIMATION_DATA_VECTOR_H_
