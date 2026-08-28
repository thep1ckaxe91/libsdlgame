#include <gtest/gtest.h>
#include "engine.hpp"
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <iostream>

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

// ---------------------------------------------------------
// White-box tests targeting specific branches and internals
// ---------------------------------------------------------

TEST(EngineTest, InitPrintsSuccessMessage) {
    // Verify that the success branch of SDL_Init outputs the correct message to std::cout.
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
    
    EXPECT_NO_THROW(sdlgame::init());
    
    std::cout.rdbuf(oldCout); // Restore original std::cout
    
    EXPECT_NE(buffer.str().find("SDL successfully initialized\n"), std::string::npos);
    
    sdlgame::quit();
}

TEST(EngineDeathTest, InitFailsAndTerminates) {
    // Force SDL_Init to fail by setting invalid driver environment variables.
    // This covers the error handling path (SDL_Init != 0) which prints to stderr and calls std::terminate().
    // We run it in a death test to safely catch the termination (and modifying env vars here is safe as EXPECT_DEATH forks).
    EXPECT_DEATH({
        setenv("SDL_VIDEODRIVER", "invalid_driver_name_to_force_failure", 1);
        setenv("SDL_AUDIODRIVER", "invalid_driver_name_to_force_failure", 1);
        sdlgame::init();
    }, "Error initializing SDL");
}

TEST(EngineTest, GetBasePathCachedResult) {
    // White-box test for get_base_path caching mechanism.
    // It should use the static 'p' variable and not query SDL_GetBasePath again.
    // We can indirectly verify this by checking if it returns the exact same object value.
    std::filesystem::path path1 = sdlgame::get_base_path();
    std::filesystem::path path2 = sdlgame::get_base_path();
    
    EXPECT_EQ(path1, path2);
}
