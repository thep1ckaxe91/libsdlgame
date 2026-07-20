#include <gtest/gtest.h>
#include "math.hpp"
#include <cmath>

using namespace sdlgame::math;

TEST(MathTest, DegreeRadianConversion) {
    EXPECT_NEAR(degree_to_radian(180.0), M_PI, 1e-5);
    EXPECT_NEAR(radian_to_degree(M_PI), 180.0, 1e-5);
}

TEST(MathTest, Clamp) {
    EXPECT_DOUBLE_EQ(clamp(5.0, 1.0, 10.0), 5.0);
    EXPECT_DOUBLE_EQ(clamp(-5.0, 1.0, 10.0), 1.0);
    EXPECT_DOUBLE_EQ(clamp(15.0, 1.0, 10.0), 10.0);
}

TEST(Vector2Test, ConstructorsAndEquality) {
    Vector2 v1(3.0, 4.0);
    EXPECT_DOUBLE_EQ(v1.x, 3.0);
    EXPECT_DOUBLE_EQ(v1.y, 4.0);

    Vector2 v2(3.0, 4.0);
    EXPECT_TRUE(v1 == v2);

    Vector2 v3(1.0, 2.0);
    EXPECT_FALSE(v1 == v3);
}

TEST(Vector2Test, BasicOperations) {
    Vector2 v1(3.0, 4.0);
    
    EXPECT_DOUBLE_EQ(v1.magnitude(), 5.0);
    EXPECT_DOUBLE_EQ(v1.sqr_magnitude(), 25.0);

    Vector2 v2 = v1 * 2.0;
    EXPECT_DOUBLE_EQ(v2.x, 6.0);
    EXPECT_DOUBLE_EQ(v2.y, 8.0);

    Vector2 v3 = v1 + v2;
    EXPECT_DOUBLE_EQ(v3.x, 9.0);
    EXPECT_DOUBLE_EQ(v3.y, 12.0);
    
    EXPECT_DOUBLE_EQ(v1.distance_to(v2), 5.0);

    Vector2 norm = v1.normalize();
    EXPECT_DOUBLE_EQ(norm.x, 3.0 / 5.0);
    EXPECT_DOUBLE_EQ(norm.y, 4.0 / 5.0);
    EXPECT_DOUBLE_EQ(norm.magnitude(), 1.0);
}
