#include <gtest/gtest.h>
#include "engine.hpp"
#include <filesystem>

TEST(EngineTest, InitAndQuit) {
    // Test that init and quit functions can be called without throwing exceptions.
    EXPECT_NO_THROW(sdlgame::init());
    EXPECT_NO_THROW(sdlgame::quit());
}

TEST(EngineTest, GetBasePathReturnsNonEmpty) {
    // Test that get_base_path() returns a valid path string.
    std::filesystem::path basePath = sdlgame::get_base_path();
    EXPECT_FALSE(basePath.empty());
}

TEST(EngineTest, InitQuitMultipleTimes) {
    // Test that init and quit can be called multiple times.
    EXPECT_NO_THROW({
        sdlgame::init();
        sdlgame::quit();
        sdlgame::init();
        sdlgame::quit();
    });
}
