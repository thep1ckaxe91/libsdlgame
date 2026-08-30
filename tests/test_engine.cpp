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
    // This covers the error handling path which prints to stderr and calls std::terminate().
    // We run it in a death test to safely catch the termination.
    EXPECT_DEATH({
        setenv("SDL_VIDEODRIVER", "invalid_driver_name_to_force_failure", 1);
        setenv("SDL_AUDIODRIVER", "invalid_driver_name_to_force_failure", 1);
        sdlgame::init();
    }, "FATAL: SDL Error at .*engine.cpp:.*\nFailing Expression: SDL_Init\\(SDL_INIT_EVERYTHING\\)");
}

TEST(EngineTest, GetBasePathCachedResult) {
    // White-box test for get_base_path caching mechanism.
    // It should use the static 'p' variable and not query SDL_GetBasePath again.
    // We can indirectly verify this by checking if it returns the exact same object value.
    std::filesystem::path path1 = sdlgame::get_base_path();
    std::filesystem::path path2 = sdlgame::get_base_path();
    
    EXPECT_EQ(path1, path2);
}

// ---------------------------------------------------------
// White-box tests for error handling macros internals
// ---------------------------------------------------------

TEST(EngineDeathTest, CheckSdlPtrTerminatesOnNull) {
    EXPECT_DEATH({
        sdlgame::internal::check_sdl_ptr<void*>(nullptr, "test_expr_ptr", "test_file.cpp", 42);
    }, "FATAL: SDL Error at test_file.cpp:42\nFailing to Create Resource at: test_expr_ptr");
}

TEST(EngineDeathTest, CheckSdlTerminatesOnNegative) {
    EXPECT_DEATH({
        sdlgame::internal::check_sdl(-1, "test_expr_val", "test_file.cpp", 42);
    }, "FATAL: SDL Error at test_file.cpp:42\nFailing Expression: test_expr_val");
}

TEST(EngineTest, CheckSdlPtrReturnsValidPointer) {
    int dummy = 0;
    int* ptr = &dummy;
    EXPECT_EQ(ptr, sdlgame::internal::check_sdl_ptr(ptr, "test_expr_ptr", "test_file.cpp", 42));
}

TEST(EngineTest, CheckSdlReturnsNonNegative) {
    EXPECT_EQ(0, sdlgame::internal::check_sdl(0, "test_expr_val", "test_file.cpp", 42));
    EXPECT_EQ(1, sdlgame::internal::check_sdl(1, "test_expr_val", "test_file.cpp", 42));
}
