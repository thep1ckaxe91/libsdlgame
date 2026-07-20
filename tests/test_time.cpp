#include <gtest/gtest.h>
#include "time.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

TEST(TimeTest, SimClockNow) {
    auto t1 = SimClock::now();
    std::this_thread::sleep_for(10ms);
    auto t2 = SimClock::now();
    EXPECT_GT(t2, t1);
}

TEST(TimeTest, WaitAndGetTicks) {
    // get_ticks returns a timepoint_t
    auto ticks1 = sdlgame::time::get_ticks();
    sdlgame::time::wait(std::chrono::duration<double>(0.01)); // wait 10ms
    auto ticks2 = sdlgame::time::get_ticks();
    
    // time_since_epoch should increase or stay same (depending on resolution and implementation)
    EXPECT_GE(ticks2.time_since_epoch().count(), ticks1.time_since_epoch().count());
}

TEST(TimeTest, ClockBasicOperations) {
    sdlgame::time::Clock clock;
    
    // Initial state
    auto dt = clock.delta_time();
    EXPECT_EQ(dt.count(), 0.0);
    EXPECT_EQ(clock.get_fps(), 0.0);

    // After tick
    std::this_thread::sleep_for(10ms);
    clock.tick(60.0);
    dt = clock.delta_time();
    EXPECT_GE(dt.count(), 0.0);

    // Set bullet time multiplier
    clock.set_bullettime_multiplier(50.0);
    EXPECT_GE(clock.delta_time().count(), 0.0);
}

TEST(TimeTest, FunctionStatsAvg) {
    sdlgame::time::FunctionStats stats;
    EXPECT_EQ(stats.call_count, 0);
    EXPECT_EQ(stats.total_time.count(), 0.0);
    
    stats.total_time = std::chrono::duration<double>(10.0);
    stats.call_count = 5;
    auto avg = stats.avg_time();
    EXPECT_DOUBLE_EQ(avg.count(), 2.0);
}

TEST(TimeTest, TimerAndManager) {
    auto& manager = sdlgame::time::TimerManager::instance();
    
    auto initial_calls = manager.get_stat("TestFunction").call_count;

    {
        sdlgame::time::Timer t("TestFunction");
        std::this_thread::sleep_for(5ms);
    } // Timer destructor reports stats

    auto stats = manager.get_stat("TestFunction");
    EXPECT_EQ(stats.call_count, initial_calls + 1);
    EXPECT_GT(stats.total_time.count(), 0.0);

    // Check get_all
    auto all_stats = manager.get_all();
    EXPECT_TRUE(all_stats.find("TestFunction") != all_stats.end());
}
