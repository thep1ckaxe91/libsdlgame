#include <gtest/gtest.h>
#include "random.hpp"
#include <limits>

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

TEST(RandomTest, RandIntExtremeRange) {
    int val = sdlgame::random::randint(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    EXPECT_GE(val, std::numeric_limits<int>::min());
    EXPECT_LE(val, std::numeric_limits<int>::max());
}

TEST(RandomTest, RandIntReversedExtremeRange) {
    int val = sdlgame::random::randint(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
    EXPECT_GE(val, std::numeric_limits<int>::min());
    EXPECT_LE(val, std::numeric_limits<int>::max());
}

TEST(RandomTest, RandIntZeros) {
    int val = sdlgame::random::randint(0, 0);
    EXPECT_EQ(val, 0);
}

TEST(RandomTest, RandFLimitsThorough) {
    for (int i = 0; i < 10000; ++i) {
        float val = sdlgame::random::randf();
        EXPECT_GE(val, 0.0f);
        EXPECT_LE(val, 1.0f);
    }
}

TEST(RandomTest, RandIntCrossZeroRange) {
    bool found_neg = false;
    bool found_pos = false;
    bool found_zero = false;
    for (int i = 0; i < 1000; ++i) {
        int val = sdlgame::random::randint(-1, 1);
        if (val < 0) found_neg = true;
        if (val > 0) found_pos = true;
        if (val == 0) found_zero = true;
        if (found_neg && found_pos && found_zero) break;
    }
    EXPECT_TRUE(found_neg);
    EXPECT_TRUE(found_pos);
    EXPECT_TRUE(found_zero);
}

TEST(RandomTest, RandIntDistribution) {
    const int num_samples = 10000;
    long long sum = 0;
    for (int i = 0; i < num_samples; ++i) {
        sum += sdlgame::random::randint(1, 10);
    }
    double mean = static_cast<double>(sum) / num_samples;
    EXPECT_NEAR(mean, 5.5, 0.5);
}

TEST(RandomTest, RandFDistribution) {
    const int num_samples = 10000;
    double sum = 0;
    for (int i = 0; i < num_samples; ++i) {
        sum += sdlgame::random::randf();
    }
    double mean = sum / num_samples;
    EXPECT_NEAR(mean, 0.5, 0.05);
}

