#include <gtest/gtest.h>
#include "time.hpp"
#include <thread>
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

TEST(TimeTest, SimClockProperties) {
    EXPECT_TRUE(SimClock::is_steady);
    
    auto t1 = SimClock::now();
    std::this_thread::sleep_for(5ms);
    auto t2 = SimClock::now();
    EXPECT_GT(t2, t1);
}

TEST(TimeTest, WaitAndGetTicks) {
    auto ticks1 = sdlgame::time::get_ticks();
    sdlgame::time::wait(duration_t(0.01)); // wait 10ms
    auto ticks2 = sdlgame::time::get_ticks();
    
    EXPECT_GE(ticks2.time_since_epoch().count(), ticks1.time_since_epoch().count());
}

TEST(TimeTest, ClockBasicOperations) {
    sdlgame::time::Clock clock;
    
    auto dt = clock.delta_time();
    EXPECT_DOUBLE_EQ(dt.count(), 1.0/60.0);
    EXPECT_DOUBLE_EQ(clock.get_fps(), 60.0);

    // Test default argument fps = 0
    clock.tick(); 
    dt = clock.delta_time();
    EXPECT_GE(dt.count(), 0.0);

    std::this_thread::sleep_for(5ms);
    clock.tick(60.0);
    dt = clock.delta_time();
    EXPECT_GE(dt.count(), 0.0);
}

TEST(TimeTest, ClockBulletTimeClamp) {
    sdlgame::time::Clock clock;
    
    auto initial_dt = clock.delta_time().count();
    
    clock.set_bullettime_multiplier(50.0);
    EXPECT_DOUBLE_EQ(clock.delta_time().count(), initial_dt * 0.5);

    clock.set_bullettime_multiplier(-10.0);
    EXPECT_DOUBLE_EQ(clock.delta_time().count(), initial_dt * 0.01);
    
    clock.set_bullettime_multiplier(200.0);
    EXPECT_DOUBLE_EQ(clock.delta_time().count(), initial_dt * 1.0);
}

TEST(TimeTest, FunctionStatsAvg) {
    sdlgame::time::FunctionStats stats;
    EXPECT_EQ(stats.call_count, 0);
    EXPECT_DOUBLE_EQ(stats.total_time.count(), 0.0);
    
    stats.total_time = duration_t(10.0);
    stats.call_count = 5;
    auto avg = stats.avg_time();
    EXPECT_DOUBLE_EQ(avg.count(), 2.0);
}

TEST(TimeTest, WaitEdgeCases) {
    auto t1 = sdlgame::time::get_ticks();
    sdlgame::time::wait(duration_t(0.0));
    sdlgame::time::wait(duration_t(-1.0));
    auto t2 = sdlgame::time::get_ticks();
    EXPECT_GE(t2, t1);
}

TEST(TimeTest, ClockTickNegativeOrZeroFps) {
    sdlgame::time::Clock clock;
    
    auto dt1 = clock.tick(0.0);
    EXPECT_GE(dt1.count(), 0.0);
    
    auto dt2 = clock.tick(-10.0);
    EXPECT_GE(dt2.count(), 0.0);
}

TEST(TimeTest, ClockElapsedIndexWrapAround) {
    sdlgame::time::Clock clock;
    
    for (int i = 0; i < 11; ++i) {
        clock.tick(1e9); 
    }
    
    auto dt = clock.delta_time();
    EXPECT_GE(dt.count(), 0.0);
}

TEST(TimeTest, TimerConstructors) {
    auto& manager = sdlgame::time::TimerManager::instance();
    
    {
        sdlgame::time::Timer t1("LvalueString");
        sdlgame::time::Timer t2(std::string("RvalueString"));
        const char* c_str = "CString";
        sdlgame::time::Timer t3(c_str);
        
        std::string move_target = "MoveString";
        sdlgame::time::Timer t4(std::move(move_target));
    }
    
    auto all_stats = manager.get_all();
    EXPECT_TRUE(all_stats.find("LvalueString") != all_stats.end());
    EXPECT_TRUE(all_stats.find("RvalueString") != all_stats.end());
    EXPECT_TRUE(all_stats.find("CString") != all_stats.end());
    EXPECT_TRUE(all_stats.find("MoveString") != all_stats.end());
}

TEST(TimeTest, TimerManagerGetStatBranches) {
    auto& manager = sdlgame::time::TimerManager::instance();
    
    auto& stat_new = manager.get_stat("NewStat");
    EXPECT_EQ(stat_new.call_count, 0);
    
    stat_new.call_count = 5;
    auto& stat_existing = manager.get_stat("NewStat");
    EXPECT_EQ(stat_existing.call_count, 5);
}

TEST(TimeTest, TimerManagerReportStats) {
    auto& manager = sdlgame::time::TimerManager::instance();
    auto dur1 = duration_t(1.0);
    auto dur2 = duration_t(0.5);
    auto dur3 = duration_t(2.0);
    
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
    
    auto avg = stats.avg_time();
    EXPECT_TRUE(std::isnan(avg.count()));
}

TEST(TimeTest, TimerAndManager) {
    auto& manager = sdlgame::time::TimerManager::instance();
    
    auto initial_calls = manager.get_stat("TestFunction").call_count;

    {
        sdlgame::time::Timer t("TestFunction");
        std::this_thread::sleep_for(5ms);
    }

    auto stats = manager.get_stat("TestFunction");
    EXPECT_EQ(stats.call_count, initial_calls + 1);
    EXPECT_GT(stats.total_time.count(), 0.0);

    auto all_stats = manager.get_all();
    EXPECT_TRUE(all_stats.find("TestFunction") != all_stats.end());
}

TEST(TimeTest, ClockGetFpsTypical) {
    sdlgame::time::Clock clock;
    for(int i = 0; i < 20; ++i) {
        clock.tick(30.0);
    }
    double fps = clock.get_fps();
    EXPECT_GT(fps, 0.0);
    EXPECT_NEAR(fps, 30.0, 5.0); // Allow some margin due to thread scheduling inaccuracies
}

TEST(TimeTest, TimerManagerSingleton) {
    auto& manager1 = sdlgame::time::TimerManager::instance();
    auto& manager2 = sdlgame::time::TimerManager::instance();
    EXPECT_EQ(&manager1, &manager2);
}
