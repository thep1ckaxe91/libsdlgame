#include <gtest/gtest.h>
#include "key.hpp"

TEST(KeyTest, InitCallable) {
    // Tests that init() can be called without exception.
    // Note: Actual SDL initialization context might be missing in unit tests,
    // but we only verify the API signature here.
    EXPECT_NO_THROW({
        sdlgame::key::init();
    });
}

TEST(KeyTest, GetPressedReturnsSpan) {
    // Tests that get_pressed() returns a valid span (could be empty or not, depending on state)
    EXPECT_NO_THROW({
        auto pressed_keys = sdlgame::key::get_pressed();
        
        // Ensure that we can access the span properties
        size_t size = pressed_keys.size();
        if (size > 0) {
            auto first_element = pressed_keys[0];
            (void)first_element;
        }
    });
}
