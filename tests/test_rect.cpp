#include <gtest/gtest.h>
#include "rect.hpp"

TEST(RectTest, BasicInitialization) {
    sdlgame::rect::Rect r(10.0, 20.0, 30.0, 40.0);
    EXPECT_DOUBLE_EQ(r.getLeft(), 10.0);
    EXPECT_DOUBLE_EQ(r.getTop(), 20.0);
    EXPECT_DOUBLE_EQ(r.getWidth(), 30.0);
    EXPECT_DOUBLE_EQ(r.getHeight(), 40.0);
}

TEST(RectTest, Contains) {
    sdlgame::rect::Rect outer(0.0, 0.0, 100.0, 100.0);
    sdlgame::rect::Rect inner(10.0, 10.0, 50.0, 50.0);
    sdlgame::rect::Rect outside(150.0, 150.0, 10.0, 10.0);

    EXPECT_TRUE(outer.contains(inner));
    EXPECT_FALSE(outer.contains(outside));
}
