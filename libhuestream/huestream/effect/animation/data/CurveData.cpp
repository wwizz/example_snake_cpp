/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/util/HueMath.h>
#include <huestream/effect/animation/data/CurveData.h>

#include <assert.h>

#include <iostream>
#include <string>

namespace huestream {


    PROP_IMPL(CurveData, Nullable<CurveOptions>, options, Options);
    PROP_IMPL(CurveData, PointListPtr, points, Points);

    CurveData::CurveData() :
            _options(Nullable<CurveOptions>()),
            _points(NEW_LIST_PTR(Point)) {
    }

    CurveData::CurveData(PointListPtr points, Nullable<CurveOptions> options) :
            _options(options),
            _points(NEW_LIST_PTR(Point)) {
        if (points != nullptr) {
            for (auto point : *points) {
                _points->push_back(point);
            }
        }
    }

    void CurveData::CorrectPoint(PointPtr p) const {
        if (!_options.has_value()) {
            return;
        }
        auto factor = _options.get_value().GetMultiplyFactor();
        p->SetY(p->GetY() * factor);

        if (_options.get_value().GetClipMin().has_value()) {
            auto clip = _options.get_value().GetClipMin().get_value();
            if (p->GetY() < clip)
                p->SetY(clip);
        }

        if (_options.get_value().GetClipMax().has_value()) {
            auto clip = _options.get_value().GetClipMax().get_value();
            if (p->GetY() > clip)
                p->SetY(clip);
        }
    }

    double CurveData::GetLength() const {
        if (!HasPoints())
            return 0;

        return GetEnd() - GetBegin() + 1;
    }

    double CurveData::GetBegin() const {
        if (!HasPoints()) return 0;
        return _points->front()->GetX();
    }

    double CurveData::GetEnd() const {
        if (!HasPoints()) return 0;
        return _points->back()->GetX();
    }

    bool CurveData::HasPoints() const {
        return _points->size() > 0;
    }

    bool CurveData::GetStartPointIndex(double x, unsigned int* index) const {
        auto found = false;

        for (unsigned int i = 0; i < _points->size(); i++) {
            if ((*_points)[i]->GetX() <= x) {
                *index = i;
                found = true;
            } else {
                break;
            }
        }

        return found;
    }

    bool CurveData::GetStartAndEndPoint(double x, PointPtr* start, PointPtr* end) const {
        unsigned int pointIdx;
        if (!GetStartPointIndex(x, &pointIdx)) return false;
        if (pointIdx == _points->size() - 1) return false;
        *start = _points->at(pointIdx);
        *end = (*_points)[pointIdx + 1];
        return true;
    }

    PointPtr CurveData::GetInterpolated(double x) const {
        if (_points->size() < 2) return _points->front();
        if (x <= GetBegin()) return _points->front();
        if (x >= GetEnd()) return _points->back();

        PointPtr start;
        PointPtr end;

        GetStartAndEndPoint(x, &start, &end);
        auto time = end->GetX() - start->GetX();
        auto absX = x - start->GetX();
        auto beginValue = start->GetY();
        auto endValue = end->GetY();
        auto interpolatedValue = HueMath::linearTween(absX, beginValue, endValue, time);

        auto p = NEW_PTR(Point, x, interpolatedValue);
        p->SetX(x);
        p->SetY(interpolatedValue);
        CorrectPoint(p);
        return p;
    }

    const CurveData &CurveData::Append(const CurveData &other) const {
        auto offsetX = 0.0;

        if (_points->size()) {
            offsetX = _points->back()->GetX();
        }

        for (auto point : *other._points) {
            _points->push_back(NEW_PTR(Point, point->GetX() + offsetX, point->GetY()));
        }

        return *this;
    }


    std::string CurveData::GetTypeName() const {
        return type;
    }

    void CurveData::Serialize(JSONNode *node) const {
        Serializable::Serialize(node);

        if (_options.has_value()) {
            JSONNode v(JSON_NODE);
            _options.get_value().Serialize(&v);
            v.set_name(AttributeOptions);
            node->push_back(v);
        }

        JSONNode arrayNode(JSON_ARRAY);
        for (size_t i = 0; i < _points->size(); ++i) {
            JSONNode v;
            auto p = _points->at(i);
            p->Serialize(&v);
            arrayNode.push_back(v);
        }
        arrayNode.set_name(AttributePoints);
        node->push_back(arrayNode);
    }

    void CurveData::Deserialize(JSONNode *node) {
        Serializable::Deserialize(node);

        _options.clear_value();
        if (SerializerHelper::IsAttributeSet(node, AttributeOptions)) {
            auto j = (*node)[AttributeOptions];
            auto o = CurveOptions();
            o.Deserialize(&j);
            _options.set_value(o);
        }

        _points->clear();
        if (SerializerHelper::IsAttributeSet(node, AttributePoints)) {
            auto j = (*node)[AttributePoints];
            for (auto pointIt = j.begin(); pointIt != j.end(); ++pointIt) {
                auto pointJ = *pointIt;
                auto p = NEW_PTR(Point);
                p->Deserialize(&pointJ);
                _points->push_back(p);
            }
        }
    }
}  // namespace huestream
