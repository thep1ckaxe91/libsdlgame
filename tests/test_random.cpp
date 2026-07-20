#include <gtest/gtest.h>
#include "random.hpp"

TEST(RandomTest, RandIntNormalRange) {
    int val = sdlgame::random::randint(1, 10);
    EXPECT_GE(val, 1);
    EXPECT_LE(val, 10);
}

TEST(RandomTest, RandIntReversedRange) {
    int val = sdlgame::random::randint(10, 1);
    EXPECT_GE(val, 1);
    EXPECT_LE(val, 10);
}

TEST(RandomTest, RandIntSameRange) {
    int val = sdlgame::random::randint(5, 5);
    EXPECT_EQ(val, 5);
}

TEST(RandomTest, RandF) {
    float val = sdlgame::random::randf();
    EXPECT_GE(val, 0.0f);
    EXPECT_LE(val, 1.0f);
}
