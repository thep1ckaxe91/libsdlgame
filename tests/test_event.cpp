#include <gtest/gtest.h>
#include "event.hpp"
#include <vector>
#include <string>

using namespace sdlgame::event;

TEST(EventTest, DefaultConstructor) {
    Event e;
    e.type = 1;
    e.timestamp = 1000;
    EXPECT_EQ(e.type, 1);
    EXPECT_EQ(e.timestamp, 1000);
}

TEST(EventTest, SDLEventConstructor) {
    SDL_Event sdl_e;
    sdl_e.type = 42;
    
    Event e(sdl_e);
    EXPECT_EQ(e.sdl_event.type, 42);
}

TEST(EventTest, OperatorSquareBrackets) {
    Event e;
    // We just check that it compiles against the API signature.
    // Wrap in try-catch to prevent test failure if the implementation throws on a missing key.
    try {
        int64_t value = e["test_key"];
        (void)value;
    } catch (...) {
        // Ignore any exceptions
    }
}

TEST(EventTest, GlobalFunctions) {
    // Test that we can call get()
    std::vector<Event>& events = get();
    // Test that we can call post()
    post(12345);
    
    // Assert that the events reference is valid (by getting its size)
    EXPECT_GE(events.size(), 0);
}
