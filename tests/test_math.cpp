#include <gtest/gtest.h>
#include "math.hpp"
#include <SDL2/SDL.h>
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

TEST(MathTest, ClampWhiteBox) {
    // Branch where left > right (swaps them)
    EXPECT_DOUBLE_EQ(clamp(5.0, 10.0, 1.0), 5.0);
    EXPECT_DOUBLE_EQ(clamp(15.0, 10.0, 1.0), 10.0);
    EXPECT_DOUBLE_EQ(clamp(-5.0, 10.0, 1.0), 1.0);
}

TEST(Vector2Test, SDLConstructorsAndConversions) {
    SDL_Point p{10, -5};
    Vector2 v(p);
    EXPECT_DOUBLE_EQ(v.x, 10.0);
    EXPECT_DOUBLE_EQ(v.y, -5.0);

    SDL_FPoint fp = v.to_SDL_FPoint();
    EXPECT_FLOAT_EQ(fp.x, 10.0f);
    EXPECT_FLOAT_EQ(fp.y, -5.0f);
}

TEST(Vector2Test, MathOperatorsAndAssignment) {
    Vector2 v1(2.0, 3.0);
    Vector2 v2(1.0, 1.0);
    
    v1 += v2;
    EXPECT_DOUBLE_EQ(v1.x, 3.0);
    EXPECT_DOUBLE_EQ(v1.y, 4.0);
    
    v1 -= v2;
    EXPECT_DOUBLE_EQ(v1.x, 2.0);
    EXPECT_DOUBLE_EQ(v1.y, 3.0);
    
    v1 *= 2.0;
    EXPECT_DOUBLE_EQ(v1.x, 4.0);
    EXPECT_DOUBLE_EQ(v1.y, 6.0);
    
    v1 /= 2.0;
    EXPECT_DOUBLE_EQ(v1.x, 2.0);
    EXPECT_DOUBLE_EQ(v1.y, 3.0);

    Vector2 neg = -v1;
    EXPECT_DOUBLE_EQ(neg.x, -2.0);
    EXPECT_DOUBLE_EQ(neg.y, -3.0);

    Vector2 diff = v1 - v2;
    EXPECT_DOUBLE_EQ(diff.x, 1.0);
    EXPECT_DOUBLE_EQ(diff.y, 2.0);

    Vector2 scalar_first = 3.0 * v1;
    EXPECT_DOUBLE_EQ(scalar_first.x, 6.0);
    EXPECT_DOUBLE_EQ(scalar_first.y, 9.0);
}

TEST(Vector2Test, WhiteBoxEdgeCases) {
    // normalize when magnitude is 0
    Vector2 zero(0.0, 0.0);
    Vector2 norm_zero = zero.normalize();
    EXPECT_DOUBLE_EQ(norm_zero.x, 0.0);
    EXPECT_DOUBLE_EQ(norm_zero.y, 0.0);

    // normalize_ip when magnitude is 0
    zero.normalize_ip();
    EXPECT_DOUBLE_EQ(zero.x, 0.0);
    EXPECT_DOUBLE_EQ(zero.y, 0.0);

    // normalize_ip normal case
    Vector2 v(3.0, 4.0);
    v.normalize_ip();
    EXPECT_DOUBLE_EQ(v.x, 3.0 / 5.0);
    EXPECT_DOUBLE_EQ(v.y, 4.0 / 5.0);

    // angle_to when magnitude is 0
    EXPECT_DOUBLE_EQ(Vector2(0, 0).angle_to(Vector2(1, 1)), 0.0);
    EXPECT_DOUBLE_EQ(Vector2(1, 1).angle_to(Vector2(0, 0)), 0.0);

    // angle_to clamping (parallel and anti-parallel)
    EXPECT_NEAR(Vector2(1, 0).angle_to(Vector2(2, 0)), 0.0, 1e-5);
    EXPECT_NEAR(Vector2(1, 0).angle_to(Vector2(-2, 0)), 180.0, 1e-5);

    // project when normal magnitude is 0
    Vector2 proj_zero = Vector2(1, 1).project(Vector2(0, 0));
    EXPECT_DOUBLE_EQ(proj_zero.x, 0.0);
    EXPECT_DOUBLE_EQ(proj_zero.y, 0.0);
}

TEST(Vector2Test, ComplexMathLogic) {
    Vector2 v1(2.0, 3.0);
    Vector2 v2(-1.0, 4.0);

    // dot product
    EXPECT_DOUBLE_EQ(v1.dot(v2), 10.0);

    // rotation
    Vector2 rot = Vector2(1.0, 0.0).rotate(90.0);
    EXPECT_NEAR(rot.x, 0.0, 1e-5);
    EXPECT_NEAR(rot.y, 1.0, 1e-5);

    // rotation in-place
    Vector2 v_rot(1.0, 0.0);
    v_rot.rotate_ip(90.0);
    EXPECT_NEAR(v_rot.x, 0.0, 1e-5);
    EXPECT_NEAR(v_rot.y, 1.0, 1e-5);

    // reflect
    Vector2 incident(1.0, -1.0);
    Vector2 normal(0.0, 1.0);
    Vector2 reflection = incident.reflect(normal);
    EXPECT_DOUBLE_EQ(reflection.x, 1.0);
    EXPECT_DOUBLE_EQ(reflection.y, 1.0);

    // reflect in-place
    incident.reflect_ip(normal);
    EXPECT_DOUBLE_EQ(incident.x, 1.0);
    EXPECT_DOUBLE_EQ(incident.y, 1.0);

    // project
    Vector2 p1(2.0, 2.0);
    Vector2 p2(1.0, 0.0);
    Vector2 projection = p1.project(p2);
    EXPECT_DOUBLE_EQ(projection.x, 2.0);
    EXPECT_DOUBLE_EQ(projection.y, 0.0);

    // project in-place
    p1.project_ip(p2);
    EXPECT_DOUBLE_EQ(p1.x, 2.0);
    EXPECT_DOUBLE_EQ(p1.y, 0.0);
}

TEST(Vector2Test, StringFormatting) {
    Vector2 v(1.5, -2.5);
    std::string expected = "Vector2<" + std::to_string(1.5) + " , " + std::to_string(-2.5) + ">";
    EXPECT_EQ(v.toString(), expected);
}

TEST(MathTest, DegreeRadianEdgeCases) {
    EXPECT_DOUBLE_EQ(degree_to_radian(0.0), 0.0);
    EXPECT_DOUBLE_EQ(degree_to_radian(-180.0), -M_PI);
    EXPECT_DOUBLE_EQ(degree_to_radian(360.0), 2 * M_PI);

    EXPECT_DOUBLE_EQ(radian_to_degree(0.0), 0.0);
    EXPECT_DOUBLE_EQ(radian_to_degree(-M_PI), -180.0);
    EXPECT_DOUBLE_EQ(radian_to_degree(2 * M_PI), 360.0);
}

TEST(MathTest, ClampEdgeCases) {
    EXPECT_DOUBLE_EQ(clamp(5.0, 5.0, 5.0), 5.0);
    EXPECT_DOUBLE_EQ(clamp(4.0, 5.0, 5.0), 5.0);
    EXPECT_DOUBLE_EQ(clamp(6.0, 5.0, 5.0), 5.0);
}

TEST(Vector2Test, MathOperatorsDivisionByZero) {
    Vector2 v1(2.0, 3.0);
    v1 /= 0.0;
    EXPECT_TRUE(std::isinf(v1.x));
    EXPECT_TRUE(std::isinf(v1.y));
}

TEST(Vector2Test, NormalizationEdgeCases) {
    Vector2 v_norm(1.0, 0.0);
    v_norm.normalize_ip();
    EXPECT_DOUBLE_EQ(v_norm.x, 1.0);
    EXPECT_DOUBLE_EQ(v_norm.y, 0.0);
}

TEST(Vector2Test, AngleToAdditionalEdgeCases) {
    EXPECT_DOUBLE_EQ(Vector2(0, 0).angle_to(Vector2(0, 0)), 0.0);

    EXPECT_DOUBLE_EQ(Vector2(1, 0).angle_to(Vector2(0, 1)), 90.0);
    EXPECT_DOUBLE_EQ(Vector2(1, 0).angle_to(Vector2(0, -1)), 90.0);

    EXPECT_DOUBLE_EQ(Vector2(1, 1).angle_to(Vector2(1, 1)), 0.0);
}

TEST(Vector2Test, RotateEdgeCases) {
    Vector2 v(1.0, 0.0);

    Vector2 rot_0 = v.rotate(0.0);
    EXPECT_DOUBLE_EQ(rot_0.x, 1.0);
    EXPECT_DOUBLE_EQ(rot_0.y, 0.0);

    Vector2 rot_360 = v.rotate(360.0);
    EXPECT_NEAR(rot_360.x, 1.0, 1e-10);
    EXPECT_NEAR(rot_360.y, 0.0, 1e-10);

    Vector2 rot_neg_90 = v.rotate(-90.0);
    EXPECT_NEAR(rot_neg_90.x, 0.0, 1e-10);
    EXPECT_NEAR(rot_neg_90.y, -1.0, 1e-10);
}

TEST(Vector2Test, DistanceEdgeCases) {
    Vector2 v(1.0, 1.0);
    EXPECT_DOUBLE_EQ(v.distance_to(v), 0.0);

    Vector2 v2(-1.0, -1.0);
    EXPECT_DOUBLE_EQ(v.distance_to(v2), std::sqrt(8.0));
}

TEST(Vector2Test, ReflectAndProjectEdgeCases) {
    Vector2 p1(2.0, 2.0);
    Vector2 normal_zero(0.0, 0.0);
    
    p1.project_ip(normal_zero);
    EXPECT_DOUBLE_EQ(p1.x, 0.0);
    EXPECT_DOUBLE_EQ(p1.y, 0.0);

    Vector2 incident(1.0, 1.0);
    Vector2 refl_zero = incident.reflect(normal_zero);
    EXPECT_DOUBLE_EQ(refl_zero.x, 1.0);
    EXPECT_DOUBLE_EQ(refl_zero.y, 1.0);
}
