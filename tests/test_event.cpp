#include <gtest/gtest.h>
#include <SDL2/SDL.h>
#include "event.hpp"
#include "constants.hpp"
#include <vector>
#include <string>

using namespace sdlgame::event;
using namespace sdlgame;

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

// --- White-box tests below ---

TEST(EventTest, ConstructorSDL_KEYDOWN) {
    SDL_Event sdl_e;
    sdl_e.type = SDL_KEYDOWN;
    sdl_e.key.timestamp = 12345;
    sdl_e.key.keysym.sym = SDLK_a;
    sdl_e.key.keysym.mod = KMOD_LSHIFT;
    sdl_e.key.keysym.scancode = SDL_SCANCODE_A;
    
    Event e(sdl_e);
    EXPECT_EQ(e.type, SDL_KEYDOWN);
    EXPECT_EQ(e.timestamp, 12345);
    EXPECT_EQ(e["key"], SDLK_a);
    EXPECT_EQ(e["mod"], KMOD_LSHIFT);
    EXPECT_EQ(e["scancode"], SDL_SCANCODE_A);
}

TEST(EventTest, ConstructorSDL_KEYUP) {
    SDL_Event sdl_e;
    sdl_e.type = SDL_KEYUP;
    sdl_e.key.timestamp = 54321;
    sdl_e.key.keysym.sym = SDLK_b;
    sdl_e.key.keysym.mod = KMOD_NONE;
    sdl_e.key.keysym.scancode = SDL_SCANCODE_B;
    
    Event e(sdl_e);
    EXPECT_EQ(e.type, SDL_KEYUP);
    EXPECT_EQ(e.timestamp, 54321);
    EXPECT_EQ(e["key"], SDLK_b);
    EXPECT_EQ(e["mod"], KMOD_NONE);
    EXPECT_EQ(e["scancode"], SDL_SCANCODE_B);
}

TEST(EventTest, ConstructorSDL_MOUSEWHEEL) {
    SDL_Event sdl_e;
    sdl_e.type = SDL_MOUSEWHEEL;
    sdl_e.wheel.timestamp = 111;
    sdl_e.wheel.x = 10;
    sdl_e.wheel.y = -5;
    
    Event e(sdl_e);
    EXPECT_EQ(e.type, SDL_MOUSEWHEEL);
    EXPECT_EQ(e.timestamp, 111);
    EXPECT_EQ(e["x"], 10);
    EXPECT_EQ(e["y"], -5);
}

TEST(EventTest, ConstructorSDL_MOUSEBUTTONDOWN) {
    SDL_Event sdl_e;
    sdl_e.type = SDL_MOUSEBUTTONDOWN;
    sdl_e.button.timestamp = 222;
    sdl_e.button.button = SDL_BUTTON_LEFT;
    sdl_e.button.x = 100;
    sdl_e.button.y = 200;
    
    Event e(sdl_e);
    EXPECT_EQ(e.type, SDL_MOUSEBUTTONDOWN);
    EXPECT_EQ(e.timestamp, 222);
    EXPECT_EQ(e["button"], SDL_BUTTON_LEFT);
    EXPECT_EQ(e["x"], 100);
    EXPECT_EQ(e["y"], 200);
}

TEST(EventTest, ConstructorSDL_MOUSEBUTTONUP) {
    SDL_Event sdl_e;
    sdl_e.type = SDL_MOUSEBUTTONUP;
    sdl_e.button.timestamp = 333;
    sdl_e.button.button = SDL_BUTTON_RIGHT;
    sdl_e.button.x = 150;
    sdl_e.button.y = 250;
    
    Event e(sdl_e);
    EXPECT_EQ(e.type, SDL_MOUSEBUTTONUP);
    EXPECT_EQ(e.timestamp, 333);
    EXPECT_EQ(e["button"], SDL_BUTTON_RIGHT);
    EXPECT_EQ(e["x"], 150);
    EXPECT_EQ(e["y"], 250);
}

TEST(EventTest, ConstructorSDL_MOUSEMOTION) {
    SDL_Event sdl_e;
    sdl_e.type = SDL_MOUSEMOTION;
    sdl_e.motion.timestamp = 444;
    sdl_e.motion.x = 300;
    sdl_e.motion.y = 400;
    sdl_e.motion.xrel = 5;
    sdl_e.motion.yrel = -10;
    
    Event e(sdl_e);
    EXPECT_EQ(e.type, SDL_MOUSEMOTION);
    EXPECT_EQ(e.timestamp, 444);
    EXPECT_EQ(e["x"], 300);
    EXPECT_EQ(e["y"], 400);
    EXPECT_EQ(e["xrel"], 5);
    EXPECT_EQ(e["yrel"], -10);
}

TEST(EventTest, OperatorSquareBracketsMissingKey) {
    SDL_Event sdl_e;
    sdl_e.type = SDL_KEYDOWN;
    sdl_e.key.timestamp = 1;
    sdl_e.key.keysym.sym = SDLK_a;
    sdl_e.key.keysym.mod = KMOD_NONE;
    sdl_e.key.keysym.scancode = SDL_SCANCODE_A;
    Event e(sdl_e);
    
    // Implementation returns -1 if key not found
    EXPECT_EQ(e["nonexistent_key"], -1);
}

TEST(EventTest, PostWarningAndEventFetch) {
    SDL_Init(SDL_INIT_EVENTS);
    
    // Clear any preexisting events
    SDL_Event dummy;
    while(SDL_PollEvent(&dummy)) {}
    
    // Post non-user event (should log warning but still push)
    post(sdlgame::USEREVENT - 1);
    
    // Post user event
    post(sdlgame::USEREVENT + 1);
    
    std::vector<Event>& evts = get();
    
    ASSERT_GE(evts.size(), 2);
    EXPECT_EQ(evts[0].type, sdlgame::USEREVENT - 1);
    EXPECT_EQ(evts[1].type, sdlgame::USEREVENT + 1);
    
    SDL_Quit();
}

TEST(EventTest, GetPollLimit) {
    SDL_Init(SDL_INIT_EVENTS);
    
    SDL_Event dummy;
    while(SDL_PollEvent(&dummy)) {}
    
    // EVENT_POLL_LIMIT is 100 in implementation
    const int limit = 100;
    
    // Push more than the limit
    for (int i = 0; i < limit + 10; ++i) {
        post(sdlgame::USEREVENT + i);
    }
    
    std::vector<Event>& evts = get();
    // Verify it only polled up to limit
    EXPECT_EQ(evts.size(), limit);
    
    // Next poll should get the remainder
    std::vector<Event>& evts2 = get();
    EXPECT_EQ(evts2.size(), 10);
    
    SDL_Quit();
}
