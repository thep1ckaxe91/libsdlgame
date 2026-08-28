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

TEST(RandomTest, RandIntNegativeRange) {
    int val = sdlgame::random::randint(-10, -5);
    EXPECT_GE(val, -10);
    EXPECT_LE(val, -5);
}

TEST(RandomTest, RandIntReversedNegativeRange) {
    int val = sdlgame::random::randint(-5, -10);
    EXPECT_GE(val, -10);
    EXPECT_LE(val, -5);
}

TEST(RandomTest, RandIntBoundaryCoverage) {
    bool found_min = false;
    bool found_max = false;
    for (int i = 0; i < 1000; ++i) {
        int val = sdlgame::random::randint(0, 1);
        if (val == 0) found_min = true;
        if (val == 1) found_max = true;
        if (found_min && found_max) break;
    }
    EXPECT_TRUE(found_min);
    EXPECT_TRUE(found_max);
}

TEST(RandomTest, RandFMultipleValues) {
    bool different = false;
    float val1 = sdlgame::random::randf();
    for (int i = 0; i < 100; ++i) {
        float val2 = sdlgame::random::randf();
        if (val1 != val2) {
            different = true;
            break;
        }
    }
    EXPECT_TRUE(different);
}
