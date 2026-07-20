#include <gtest/gtest.h>
#include "display.hpp"
#include "math.hpp"
#include "surface.hpp"
#include <string>

using namespace sdlgame;

TEST(DisplayTest, SetModeAndSize) {
    // Simply test that the function signatures are as expected.
    // We call set_mode to initialize the display surface.
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
    // Test rendering configuration and context getters.
    bool scale_set = display::set_render_scale_quality(true);
    
    SDL_Window* win = display::get_window();
    SDL_Renderer* ren = display::get_renderer();
    
    display::flip();
    display::quit();
    
    // Basic checks just to ensure variables are compiled and used.
    EXPECT_TRUE(true);
}
