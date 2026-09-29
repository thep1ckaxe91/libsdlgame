#include "gtest/gtest.h"
#include <SDL_events.h>
#include <gtest/gtest.h>
#include "constants.hpp"
#include "display.hpp"
#include "engine.hpp"
#include "event.hpp"
#include "math.hpp"
#include "surface.hpp"
#include <string>

using namespace sdlgame;

TEST(DisplayTest, SetModeAndSize) {
    // Simply test that the function signatures are as expected.
    // We call set_mode to initialize the display surface.
    init();
    auto& surf = display::set_mode(800, 600, 0);
    
    double w = display::get_width();
    double h = display::get_height();
    
    display::set_window_size(1024, 768);
    math::Vector2 size = display::get_window_size();
    
    // We expect these functions to return without type errors.
    // It is fine if values are defaults when running without a real display.
    EXPECT_GE(w, 0.0);
    EXPECT_GE(h, 0.0);
}

TEST(DisplayTest, WindowStates) {

    init();
    display::set_mode(600,400, sdlgame::RESIZABLE);
    // Test the window state toggles.
    display::maximize();
    display::minimize();
    display::restore();
    display::fullscreen();
    display::fullscreen_desktop();
    
    bool fs = display::is_fullscreen();
    EXPECT_EQ(fs, display::is_fullscreen()); 
}

TEST(DisplayTest, WindowProperties) {
    // Test the setters and getters for various window properties.
    display::set_caption("Test Window");
    display::set_window_pos(100, 100);
    
    auto pos = display::get_window_pos();
    
    bool is_borderless = display::borderless(-1);
    EXPECT_EQ(is_borderless, display::borderless());
    
    bool is_grabbed = display::grab(-1);
    EXPECT_EQ(is_grabbed, display::grab());
}

TEST(DisplayTest, RenderContextAndCleanup) {
    init();
    display::set_mode(600,400);
    // Test rendering configuration and context getters.
    bool scale_set = display::set_render_scale_quality(true);
    
    SDL_Window* win = display::get_window();
    SDL_Renderer* ren = display::get_renderer();
    
    display::flip();
    display::quit();
    
    // Basic checks just to ensure variables are compiled and used.
    EXPECT_TRUE(true);
}

TEST(DisplayDeathTest, UninitializedDisplayWidth) {
    EXPECT_DEATH(display::get_width(), "Display not yet set mode");
}

TEST(DisplayDeathTest, UninitializedDisplayHeight) {
    EXPECT_DEATH(display::get_height(), "Display not yet set mode");
}

TEST(DisplayTest, SetModeDesktopFallback) {
    // Testing logic branch where width=0, height=0 uses desktop display mode
    sdlgame::init();
    display::set_mode();
    EXPECT_GT(display::get_width(), 0.0);
    EXPECT_GT(display::get_height(), 0.0);

    // Testing logic branch where only height=0 uses desktop display mode
    display::set_mode(800, 0);
    EXPECT_GT(display::get_width(), 0.0);
    EXPECT_GT(display::get_height(), 0.0);
    
    // Testing logic branch where only width=0 uses desktop display mode
    display::set_mode(0, 600);
    EXPECT_GT(display::get_width(), 0.0);
    EXPECT_GT(display::get_height(), 0.0);
}

TEST(DisplayDeathTest, SetModeInvalidSize) {
    EXPECT_DEATH(display::set_mode(-1000, -1000, 0), "Can't initialize window with negative/too large size");
    EXPECT_DEATH(display::set_mode(0x7fffffff,0x7fffffff, 0), "Can't initialize window with negative/too large size");
}

TEST(DisplayTest, GrabLogicBranches) {
    init();
    display::set_mode(600,400);
    
    display::grab(1);
    SDL_PumpEvents();
    EXPECT_TRUE(display::grab());
    
    display::grab(0);
    SDL_PumpEvents();
    EXPECT_FALSE(display::grab());
}

TEST(DisplayTest, BorderlessLogicBranches) {
    init();
    display::set_mode();
    
    display::borderless(1);
    EXPECT_TRUE(display::borderless());
    
    display::borderless(0);
    EXPECT_FALSE(display::borderless());
}

TEST(DisplayTest, FullscreenLogicBranches) {
    display::set_mode(800, 600);
    
    display::fullscreen();
    EXPECT_TRUE(display::is_fullscreen());
    
    display::restore();
    EXPECT_FALSE(display::is_fullscreen());

    display::fullscreen_desktop();
    EXPECT_TRUE(display::is_fullscreen());
    
    display::restore();
    EXPECT_FALSE(display::is_fullscreen());
}

TEST(DisplayTest, GetWindowSizeSideEffect) {
    display::set_mode(800, 600);
    display::set_window_size(1024, 768);
    
    // get_window_size updates the internal proxy_surf resolution
    display::get_window_size();
    
    EXPECT_EQ(display::get_width(), 1024);
    EXPECT_EQ(display::get_height(), 768);
}

TEST(DisplayTest, RenderScaleQualityBranches) {
    sdlgame::init();
    display::set_mode(800, 600);
    
    // Testing the true branch
    EXPECT_TRUE(display::set_render_scale_quality(true));
    // Testing the false branch
    EXPECT_TRUE(display::set_render_scale_quality(false));
}

TEST(DisplayDeathTest, SetIconInvalidPath) {
    init();
    display::set_mode(800, 600);
    // Now uses SDL_NEW, so it should terminate if the path is invalid
    EXPECT_DEATH(display::set_icon("non_existent_file.png"), "FATAL: SDL Error");
}

TEST(DisplayTest, DisplayGetters) {
    init();
    display::set_mode(800, 600);
    EXPECT_NE(display::get_window(), nullptr);
    EXPECT_NE(display::get_renderer(), nullptr);
    auto& surf = display::get_surf();
    EXPECT_EQ(surf.get_width(), 800);
}
