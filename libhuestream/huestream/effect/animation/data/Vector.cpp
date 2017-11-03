/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/effect/animation/data/Vector.h>

#include <cmath>

#define PI 3.14159265

namespace huestream {

    Vector::Vector(const Point &from, const Point &to) : _x(to.GetX() - from.GetX()),
                                                         _y(to.GetY() - from.GetY()) {}

    Vector::Vector(const Location &from, const Location &to) : _x(to.GetX() - from.GetX()),
                                                               _y(to.GetY() - from.GetY()) {}

    double Vector::GetLength() const {
        return std::sqrt(_x * _x + _y * _y);
    }

    int Vector::GetQuadrant() const {
        if (_x >= 0) {
            if (_y >= 0) return 0;
            return 3;
        }
        if (_y >= 0) return 1;
        return 2;
    }

//                90
//                |
//        Q1      |     Q0
//                |
//  180 ----------|---------- 0
//                |
//        Q2      |     Q3
//                |
//               270

    double Vector::GetAngle() const {
        auto angle = std::atan2(_y, _x) * 180 / PI;

        if (angle < 0)
            angle += 360;

        return angle;
    }

    const double &Vector::get_y() const {
        return _y;
    }

    const double &Vector::get_x() const {
        return _x;
    }

}  // namespace huestream
