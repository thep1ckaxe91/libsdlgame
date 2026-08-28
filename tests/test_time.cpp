#include <gtest/gtest.h>
#include "time.hpp"
#include <thread>
#include <chrono>
#include <cmath>

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
    EXPECT_EQ(dt.count(), 1.0/60.0);
    EXPECT_EQ(clock.get_fps(), 60);

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

TEST(TimeTest, WaitEdgeCases) {
    auto t1 = sdlgame::time::get_ticks();
    sdlgame::time::wait(std::chrono::duration<double>(0.0));
    sdlgame::time::wait(std::chrono::duration<double>(-1.0));
    auto t2 = sdlgame::time::get_ticks();
    EXPECT_GE(t2, t1);
}

TEST(TimeTest, ClockTickNegativeOrZeroFps) {
    sdlgame::time::Clock clock;
    
    // fps = 0.0 should be treated as 1e9
    auto dt1 = clock.tick(0.0);
    EXPECT_GE(dt1.count(), 0.0);
    
    // fps = -10.0 should be treated as 1e9
    auto dt2 = clock.tick(-10.0);
    EXPECT_GE(dt2.count(), 0.0);
}

TEST(TimeTest, ClockElapsedIndexWrapAround) {
    sdlgame::time::Clock clock;
    
    // elapsed_times size is 10. Tick 11 times to force wrap around.
    for (int i = 0; i < 11; ++i) {
        clock.tick(1e9); 
    }
    
    auto dt = clock.delta_time();
    EXPECT_GE(dt.count(), 0.0);
}

TEST(TimeTest, ClockBulletTimeClamp) {
    sdlgame::time::Clock clock;
    
    auto initial_dt = clock.delta_time().count();
    
    clock.set_bullettime_multiplier(-10.0);
    EXPECT_DOUBLE_EQ(clock.delta_time().count(), initial_dt * 0.01);
    
    clock.set_bullettime_multiplier(200.0);
    EXPECT_DOUBLE_EQ(clock.delta_time().count(), initial_dt * 1.0);
}

TEST(TimeTest, TimerConstructors) {
    auto& manager = sdlgame::time::TimerManager::instance();
    
    {
        sdlgame::time::Timer t1("LvalueString");
        sdlgame::time::Timer t2(std::string("RvalueString"));
        const char* c_str = "CString";
        sdlgame::time::Timer t3(c_str);
    }
    
    auto all_stats = manager.get_all();
    EXPECT_TRUE(all_stats.find("LvalueString") != all_stats.end());
    EXPECT_TRUE(all_stats.find("RvalueString") != all_stats.end());
    EXPECT_TRUE(all_stats.find("CString") != all_stats.end());
}

TEST(TimeTest, TimerManagerGetStatBranches) {
    auto& manager = sdlgame::time::TimerManager::instance();
    
    // Non-existent (creates new entry)
    auto& stat_new = manager.get_stat("NewStat");
    EXPECT_EQ(stat_new.call_count, 0);
    
    // Existent
    stat_new.call_count = 5;
    auto& stat_existing = manager.get_stat("NewStat");
    EXPECT_EQ(stat_existing.call_count, 5);
}

TEST(TimeTest, TimerManagerReportStats) {
    auto& manager = sdlgame::time::TimerManager::instance();
    auto dur1 = std::chrono::duration<double>(1.0);
    auto dur2 = std::chrono::duration<double>(0.5);
    auto dur3 = std::chrono::duration<double>(2.0);
    
    manager.report("ManualReport", dur1);
    manager.report("ManualReport", dur2);
    manager.report("ManualReport", dur3);
    
    auto stats = manager.get_stat("ManualReport");
    EXPECT_EQ(stats.call_count, 3);
    EXPECT_DOUBLE_EQ(stats.total_time.count(), 3.5);
    EXPECT_DOUBLE_EQ(stats.min_time.count(), 0.5);
    EXPECT_DOUBLE_EQ(stats.max_time.count(), 2.0);
}

TEST(TimeTest, FunctionStatsZeroCallsAvg) {
    sdlgame::time::FunctionStats stats;
    EXPECT_EQ(stats.call_count, 0);
    EXPECT_DOUBLE_EQ(stats.min_time.count(), 1e308);
    EXPECT_DOUBLE_EQ(stats.max_time.count(), 0.0);
    
    // double divided by 0 should be NaN since total_time is 0
    auto avg = stats.avg_time();
    EXPECT_TRUE(std::isnan(avg.count()));
}
