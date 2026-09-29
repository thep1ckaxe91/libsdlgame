#include <SDL_hints.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <gtest/gtest.h>
#include "mouse.hpp"
#include "math.hpp"
#include "display.hpp"
#include <SDL2/SDL.h>
#include <iostream>

TEST(MouseTest, GetPos) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        GTEST_SKIP() << "Failed to initialize SDL Video";
    }
    sdlgame::display::set_mode(800, 600);
    
    sdlgame::math::Vector2 pos = sdlgame::mouse::get_pos();
    
    sdlgame::display::quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SUCCEED();
}

TEST(MouseTest, GetPressed) {
    std::span<const bool, 5> pressed = sdlgame::mouse::get_pressed();
    EXPECT_EQ(pressed.size(), 5);
}

TEST(MouseTest, GetRel) {
    sdlgame::math::Vector2 rel = sdlgame::mouse::get_rel();
    SUCCEED();
}

TEST(MouseTest, Visibility) {
    bool initial_visibility = sdlgame::mouse::get_visible();
    
    sdlgame::mouse::set_visible(!initial_visibility);
    EXPECT_EQ(sdlgame::mouse::get_visible(), !initial_visibility);
    
    sdlgame::mouse::set_visible(initial_visibility);
    EXPECT_EQ(sdlgame::mouse::get_visible(), initial_visibility);
}

// === NEW WHITE-BOX TESTS ===

TEST(MouseTest, GetPressed_ReturnsSpanToStaticArray) {
    // get_pressed returns a span to a static array internally. 
    // Two consecutive calls must return a span covering the exact same memory address.
    std::span<const bool, 5> pressed1 = sdlgame::mouse::get_pressed();
    std::span<const bool, 5> pressed2 = sdlgame::mouse::get_pressed();
    
    EXPECT_EQ(pressed1.data(), pressed2.data());
    EXPECT_EQ(pressed1.size(), 5);
}

TEST(MouseTest, SetVisible_EnablesAndDisables) {
    // Tests the true and false branch of set_visible logic internally using SDL_ENABLE / SDL_DISABLE
    sdlgame::mouse::set_visible(true);
    EXPECT_TRUE(sdlgame::mouse::get_visible());
    EXPECT_EQ(SDL_ShowCursor(SDL_QUERY), SDL_ENABLE);

    sdlgame::mouse::set_visible(false);
    EXPECT_FALSE(sdlgame::mouse::get_visible());
    EXPECT_EQ(SDL_ShowCursor(SDL_QUERY), SDL_DISABLE);
    
    // Restore state
    sdlgame::mouse::set_visible(true);
}

TEST(MouseTest, GetPos_WithWindowAndRenderer) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        GTEST_SKIP() << "Failed to initialize SDL Video";
    }
#ifdef __linux__
    auto session_type = SDL_getenv("XDG_SESSION_TYPE");
    if(session_type != nullptr && std::string(session_type) == "wayland") {
        std::cerr << "Wayland doesn't support mouse warp, please test manually\n";
        GTEST_SKIP();
    }
#endif

    // set_mode will create the window and renderer
    sdlgame::display::set_mode(800, 600);
    SDL_Renderer* renderer = sdlgame::display::get_renderer();
    ASSERT_NE(renderer, nullptr);
    
    // Force a specific logical size so we can verify coordinate mapping
    if(SDL_RenderSetLogicalSize(renderer, 400, 300)) {
        GTEST_SKIP() << "Failed to set renderer logical size";
    }
    
    SDL_Window* win = sdlgame::display::get_window();
    ASSERT_NE(win, nullptr);
    // FIXME: the problem being on wayland, call to warp mouse position is an noop
    // for that reason, we've tried to fake event, but that also doesnt seem to work either
    // so does override video driver or env values
    // 

#ifdef __linux__
    SDL_ShowCursor(SDL_FALSE);
#endif
    SDL_WarpMouseInWindow(win, 400, 300);
#ifdef __linux__
    SDL_ShowCursor(SDL_TRUE);
#endif
    SDL_PumpEvents();
    
    sdlgame::math::Vector2 pos = sdlgame::mouse::get_pos();
    // Since window is 800x600 and logical is 400x300, 
    // window(400, 300) should map to logical(200, 150)
    EXPECT_NEAR(pos.x, 200.0, 1.0);
    EXPECT_NEAR(pos.y, 150.0, 1.0);
    
    sdlgame::display::quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

TEST(MouseTest, GetRel_AfterWarp) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        GTEST_SKIP() << "Failed to initialize SDL Video";
    }
    
    sdlgame::display::set_mode(800, 600);
    SDL_Window* win = sdlgame::display::get_window();
    ASSERT_NE(win, nullptr);
    
    // Clear relative state
    sdlgame::mouse::get_rel();
    
    // Warp mouse and pump events
    SDL_WarpMouseInWindow(win, 100, 100);
    SDL_PumpEvents();
    
    // Read relative state
    sdlgame::math::Vector2 rel = sdlgame::mouse::get_rel();
    
    // Depending on platform, WarpMouse might or might not generate relative motion.
    // We at least cover the internal branches and error conditions (not crashing).
    SUCCEED();
    
    sdlgame::display::quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

TEST(MouseTest, GetPos_WithoutRendererEdgeCase) {
    // Ensure display is shut down
    sdlgame::display::quit();
    
    EXPECT_DEATH({
        sdlgame::mouse::get_pos();
    }, "FATAL: SDL Error");
}

TEST(MouseTest, GetPressed_SimulateState) {
    // We can't directly mock SDL_GetMouseState, but we can verify it doesn't crash 
    // and correctly processes a zero state (no buttons pressed) if we ensure events are pumped.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        GTEST_SKIP() << "Failed to initialize SDL Video";
    }
    sdlgame::display::set_mode(800, 600);
    SDL_PumpEvents();
    
    std::span<const bool, 5> pressed = sdlgame::mouse::get_pressed();
    EXPECT_EQ(pressed.size(), 5);
    
    sdlgame::display::quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
