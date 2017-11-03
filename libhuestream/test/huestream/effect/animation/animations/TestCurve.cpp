#include <huestream/effect/animation/animations/CurveAnimation.h>
#include <huestream/effect/animation/data/PointHelper.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test/huestream/common/TestSerializeBase.h"

using testing::ElementsAre;

namespace huestream {

class TestCurve : public testing::Test, public TestSerializeBase {
    double _timeMs;
 protected:
    virtual double AddMillisecondsAndReturnValue(CurveAnimation *curve, double positionMs) {
        _timeMs += positionMs;
        curve->SetMarker(_timeMs);
        return curve->GetValue();
    }

    virtual void SetUp() {
        _timeMs = 0;
    }

    virtual void TearDown() {
    }
};

TEST_F(TestCurve, DurationIsCorrect) {
    auto curve = CurveAnimation(PointHelper::CreatePtr(NEW_PTR(Point, 0, 0), NEW_PTR(Point, 5000, 1)));
    EXPECT_EQ(curve.GetLength(), 5001);
}

TEST_F(TestCurve, GetLastSamplesWhenTimeIsPastCurve) {
    auto curve = CurveAnimation(PointHelper::CreatePtr(NEW_PTR(Point, 0, 10),
                                              NEW_PTR(Point, 1000, 11),
                                              NEW_PTR(Point, 2000, 12),
                                              NEW_PTR(Point, 3000, 13),
                                              NEW_PTR(Point, 4000, 14),
                                              NEW_PTR(Point, 5000, 15)));

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 5000), 15);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 6000), 15);
}

TEST_F(TestCurve, GetSamplesInBetweenWhenTimeIsWithinCurve) {
    auto curve = CurveAnimation(PointHelper::CreatePtr(
                                              NEW_PTR(Point, 0, 10),
                                              NEW_PTR(Point, 1000, 11),
                                              NEW_PTR(Point, 2000, 12),
                                              NEW_PTR(Point, 3000, 13),
                                              NEW_PTR(Point, 4000, 14),
                                              NEW_PTR(Point, 5000, 15)));
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 0), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 250), 10.25);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 250), 10.5);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 500), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 500), 11.5);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 500), 12);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 13);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 14);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 15);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 15);
}

TEST_F(TestCurve, GetSamplesInBetweenWhenTimeIsWithinCurveForOneTimeExtraWhenRepeatIsEqualToTwo) {
    auto curve = CurveAnimation(2, PointHelper::CreatePtr(NEW_PTR(Point, 0, 10), NEW_PTR(Point, 1000, 11),
                                                 NEW_PTR(Point, 1999, 11.999)));

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 0), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 999), 11.999);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 999), 11.999);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 999), 11.999);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1), 11.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 1000), 11.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curve, 10000), 11.999);
}

TEST_F(TestCurve, ExtendCurve) {
    auto curveTotal = CurveAnimation(
        PointHelper::CreatePtr(NEW_PTR(Point, 0, 10), NEW_PTR(Point, 1000, 11), NEW_PTR(Point, 2000, 12)));
    auto curveExtend = CurveAnimation(
        PointHelper::CreatePtr(NEW_PTR(Point, 0, 1), NEW_PTR(Point, 1000, 2), NEW_PTR(Point, 2000, 3)));

    EXPECT_EQ((*curveExtend.GetPoints())[0]->GetX(), 0);
    EXPECT_EQ((*curveExtend.GetPoints())[1]->GetX(), 1000);
    EXPECT_EQ((*curveExtend.GetPoints())[2]->GetX(), 2000);

    curveTotal.Append(curveExtend);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 0), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 999), 11.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1), 1);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 2);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 3);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 5000), 3);

    EXPECT_EQ((*curveExtend.GetPoints())[0]->GetX(), 0);
    EXPECT_EQ((*curveExtend.GetPoints())[1]->GetX(), 1000);
    EXPECT_EQ((*curveExtend.GetPoints())[2]->GetX(), 2000);

    EXPECT_EQ((*curveExtend.GetPoints())[0]->GetY(), 1);
    EXPECT_EQ((*curveExtend.GetPoints())[1]->GetY(), 2);
    EXPECT_EQ((*curveExtend.GetPoints())[2]->GetY(), 3);
}

TEST_F(TestCurve, ExtendCurveAndRepeat) {
    auto curveTotal = CurveAnimation(2, PointHelper::CreatePtr(NEW_PTR(Point, 0, 1), NEW_PTR(Point, 1000, 2),
                                                      NEW_PTR(Point, 2000, 3)));
    auto curveExtend = CurveData(
        PointHelper::CreatePtr(NEW_PTR(Point, 0, 10), NEW_PTR(Point, 1000, 11), NEW_PTR(Point, 1999, 11.999)));

    curveTotal.Append(curveExtend);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 0), 1);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 2);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 999), 2.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 999), 11.999);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1), 1);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 2);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 999), 2.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 999), 11.999);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1), 1);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 2);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 999), 2.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1), 10);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 11);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 999), 11.999);

    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1), 11.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 1000), 11.999);
    EXPECT_EQ(AddMillisecondsAndReturnValue(&curveTotal, 10000), 11.999);
}

TEST_F(TestCurve, InfiniteIsEndlessAndFiniteIsEnding) {
    auto endless = CurveAnimation(INF, PointHelper::CreatePtr(NEW_PTR(Point, 0, 0), NEW_PTR(Point, 1, 1)));
    auto ending = CurveAnimation(PointHelper::CreatePtr(NEW_PTR(Point, 0, 0), NEW_PTR(Point, 1, 1)));

    EXPECT_TRUE(endless.IsEndless());
    EXPECT_FALSE(ending.IsEndless());
}

TEST_F(TestCurve, Serialize) {
    auto o = Nullable<CurveOptions>(CurveOptions(2, Nullable<double>(0.1), Nullable<double>(0.2)));
    auto c = CurveAnimation(INF,
                   PointHelper::CreatePtr(NEW_PTR(Point, 1, 2), NEW_PTR(Point, 3, 4), NEW_PTR(Point, 5, 6),
                                          NEW_PTR(Point, 7, 8), NEW_PTR(Point, 9, 10)), o);
    JSONNode node;
    c.Serialize(&node);
    ASSERT_AttributeIsSetAndStringValue(node, Serializable::AttributeType, CurveAnimation::type);
    ASSERT_TRUE(SerializerHelper::IsAttributeSet(&node, CurveAnimation::AttributeCurveData));

    ASSERT_FALSE(SerializerHelper::IsAttributeSet(&node, CurveAnimation::AttributeRepeatTimes));
    ASSERT_AttributeIsSetAndAllCurveOptionsAreSet(node[CurveAnimation::AttributeCurveData], CurveData::AttributeOptions, 2, 0.1,
                                                  0.2);
    ASSERT_AttributeIsSetAndContainPoints(node[CurveAnimation::AttributeCurveData], CurveData::AttributePoints, 1, 5);
}

TEST_F(TestCurve, SerializeWithRepeatTimes) {
    auto o = Nullable<CurveOptions>(CurveOptions(2, Nullable<double>(0.1), Nullable<double>(0.2)));
    auto c = CurveAnimation(5,
                   PointHelper::CreatePtr(NEW_PTR(Point, 1, 2), NEW_PTR(Point, 3, 4), NEW_PTR(Point, 5, 6),
                                          NEW_PTR(Point, 7, 8), NEW_PTR(Point, 9, 10)), o);
    JSONNode node;
    c.Serialize(&node);
    ASSERT_AttributeIsSetAndStringValue(node, Serializable::AttributeType, CurveAnimation::type);

    ASSERT_TRUE(SerializerHelper::IsAttributeSet(&node, CurveAnimation::AttributeRepeatTimes));
    ASSERT_AttributeIsSetAndDoubleValue(node, CurveAnimation::AttributeRepeatTimes, 5);
    ASSERT_AttributeIsSetAndAllCurveOptionsAreSet(node[CurveAnimation::AttributeCurveData], CurveData::AttributeOptions, 2, 0.1,
                                                  0.2);
    ASSERT_AttributeIsSetAndContainPoints(node[CurveAnimation::AttributeCurveData], CurveData::AttributePoints, 1, 5);
}

TEST_F(TestCurve, Deserialize) {
    JSONNode node;
    AddStringAttribute(&node, Serializable::AttributeType, CurveAnimation::type);
    AddDoubleAttribute(&node, CurveAnimation::AttributeRepeatTimes, 52);
    AddCurveAttribute(&node, CurveAnimation::AttributeCurveData,
                      PointHelper::CreatePtr(NEW_PTR(Point, 1, 2), NEW_PTR(Point, 3, 4), NEW_PTR(Point, 5, 6),
                                             NEW_PTR(Point, 7, 8), NEW_PTR(Point, 9, 10)));

    auto c = CurveAnimation();
    c.Deserialize(&node);

//  ASSERT_FALSE(c.GetOptions().has_value());

    auto points = c.GetPoints();
    ASSERT_EQ(5, points->size());

    auto count = 1;
    for (auto point : *points) {
        ASSERT_EQ(count++, point->GetX());
        ASSERT_EQ(count++, point->GetY());
    }

    ASSERT_EQ(52, c.GetRepeatTimes());
}

TEST_F(TestCurve, Clone) {
    auto       c = CurveAnimation(PointHelper::CreatePtr(NEW_PTR(Point, 0, 0), NEW_PTR(Point, 5000, 1)));
    auto c_clone = c.Clone();
    ASSERT_NE(nullptr,            c_clone);
    EXPECT_EQ(c.GetValue(),       c_clone->GetValue());
    EXPECT_EQ(c.GetMarker(),      c_clone->GetMarker());
    EXPECT_EQ(c.GetTypeName(),    c_clone->GetTypeName());
    EXPECT_EQ(c.GetTotalLength(), c_clone->GetTotalLength());
}

}  // namespace huestream