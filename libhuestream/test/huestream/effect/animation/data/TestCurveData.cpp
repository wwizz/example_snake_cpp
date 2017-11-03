#include <huestream/effect/animation/data/PointHelper.h>
#include <huestream/effect/animation/data/CurveOptions.h>
#include <huestream/effect/animation/data/CurveData.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test/huestream/common/TestSerializeBase.h"
#include <vector>
#include <memory>

namespace huestream { ;

    class TestCurveData : public testing::Test, public TestSerializeBase {

    public:
        virtual void SetUp() {
        }

        virtual void TearDown() {
        }

        PointListPtr CreateTestPoints() {
			auto points = std::make_shared<PointList>();
			points->push_back(NEW_PTR(Point, 1, 2));
			points->push_back(NEW_PTR(Point, 3, 4));
			points->push_back(NEW_PTR(Point, 5, 6));
			points->push_back(NEW_PTR(Point, 7, 8));
			points->push_back(NEW_PTR(Point, 9, 10));
            return points;
        }
    };

    TEST_F(TestCurveData, Serialize) {
        auto c = CurveData(CreateTestPoints());

        JSONNode node;
        c.Serialize(&node);

        ASSERT_AttributeIsSetAndStringValue(node, Serializable::AttributeType, CurveData::type);
        ASSERT_FALSE(SerializerHelper::IsAttributeSet(&node, CurveData::AttributeOptions));
        ASSERT_AttributeIsSetAndContainPoints(node, CurveData::AttributePoints, 1, 5);
    }

    TEST_F(TestCurveData, SerializeWithOptions) {
        auto o = Nullable<CurveOptions>(CurveOptions(2, Nullable<double>(0.1), Nullable<double>(0.2)));
        auto c = CurveData(CreateTestPoints(), o);
        JSONNode node;
        c.Serialize(&node);

        ASSERT_AttributeIsSetAndStringValue(node, Serializable::AttributeType, CurveData::type);
        ASSERT_AttributeIsSetAndAllCurveOptionsAreSet(node, CurveData::AttributeOptions, 2, 0.1, 0.2);
        ASSERT_AttributeIsSetAndContainPoints(node, CurveData::AttributePoints, 1, 5);
    }

    TEST_F(TestCurveData, Deserialize) {
        JSONNode node;
        AddStringAttribute(&node, Serializable::AttributeType, CurveData::type);
        AddPointsAttribute(&node, CurveData::AttributePoints, CreateTestPoints());

        auto c = CurveData();
        c.Deserialize(&node);

        ASSERT_FALSE(c.GetOptions().has_value());

        auto points = c.GetPoints();
        ASSERT_EQ(5, points->size());

        auto count = 1;
        for (auto point : *points) {
            ASSERT_EQ(count++, point->GetX());
            ASSERT_EQ(count++, point->GetY());
        }
    }

}  // namespace huestream