#include <gtest/gtest.h>
#include <SDL2/SDL.h>
#include "engine.hpp"
#include "key.hpp"

TEST(KeyDeathTest, GetPressedWithoutInit) {
    // White-box test: Tests the internal assert in get_pressed() when keyState is null.
    // Death tests run in a forked process before other tests, ensuring init() hasn't been called.
#ifndef NDEBUG
    EXPECT_DEATH({
        sdlgame::key::get_pressed();
    }, "sdlgame::key::init\\(\\) was never called");
#endif
}

TEST(KeyDeathTest, InitWithoutSDL) {
    // White-box test: Tests the internal check in init() when keyState is null.
    // Death tests run in a forked process. 
    // Without sdlgame::init() or by explicitly quitting SDL, SDL_GetKeyboardState returns nullptr.
    EXPECT_DEATH({
        SDL_Quit(); // Ensure SDL is fully uninitialized to force nullptr
        sdlgame::key::init();
    }, "FATAL: SDL Error*");
}

TEST(KeyTest, InitCallable) {
    // Tests that init() can be called without exception.
    // Note: Actual SDL initialization context might be missing in unit tests,
    // but we only verify the API signature here.
    EXPECT_NO_THROW({
        sdlgame::init();
        sdlgame::key::init();
    });
}

TEST(KeyTest, InitIdempotent) {
    // White-box test: Testing multiple calls to init() to ensure it doesn't leak or fault.
    // Internally it just re-assigns keyState and numKeys from SDL_GetKeyboardState.
    sdlgame::init();
    EXPECT_NO_THROW({
        sdlgame::key::init();
        sdlgame::key::init();
    });
}

TEST(KeyTest, GetPressedReturnsSpan) {
    sdlgame::init();
    sdlgame::key::init();
    // Tests that get_pressed() returns a valid span (could be empty or not, depending on state)
    EXPECT_NO_THROW({
        std::span<const uint8_t> pressed_keys = sdlgame::key::get_pressed();
        
        // Ensure that we can access the span properties
        if (!pressed_keys.empty()) {
            auto first_element = pressed_keys.front();
            (void)first_element;
        }
    });
}

TEST(KeyTest, GetPressedHasValidSize) {
    // White-box test: Checks that numKeys was correctly populated.
    // After initialization, numKeys (and thus span size) should be > 0.
    sdlgame::init();
    sdlgame::key::init();
    std::span<const uint8_t> pressed_keys = sdlgame::key::get_pressed();
    EXPECT_GT(pressed_keys.size(), 0) << "Expected internal numKeys to be updated to a positive value.";
}

TEST(KeyTest, GetPressedSpanBoundary) {
    // White-box test: Check loop/access boundaries of the internal span.
    sdlgame::init();
    sdlgame::key::init();
    std::span<const uint8_t> pressed_keys = sdlgame::key::get_pressed();
    
    // Ensure we can safely access the very last element without out-of-bounds fault
    if (!pressed_keys.empty()) {
        EXPECT_NO_THROW({
            volatile uint8_t last_key = pressed_keys.back();
            (void)last_key;
        });
    }
}

TEST(KeyTest, GetPressedDataNotNull) {
    // White-box test: verifies the internal keyState pointer is non-null after init
    sdlgame::init();
    sdlgame::key::init();
    std::span<const uint8_t> pressed_keys = sdlgame::key::get_pressed();
    EXPECT_NE(pressed_keys.data(), nullptr) << "keyState pointer should not be null after init()";
}
