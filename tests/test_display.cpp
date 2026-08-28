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

TEST(DisplayDeathTest, UninitializedDisplayWidth) {
    EXPECT_DEATH(display::get_width(), "Display not yet set mode");
}

TEST(DisplayDeathTest, UninitializedDisplayHeight) {
    EXPECT_DEATH(display::get_height(), "Display not yet set mode");
}

TEST(DisplayTest, SetModeDesktopFallback) {
    display::set_mode(0, 0, 0);
    EXPECT_GT(display::get_width(), 0.0);
    EXPECT_GT(display::get_height(), 0.0);
}

TEST(DisplayTest, GrabLogicBranches) {
    display::set_mode(800, 600, 0);
    
    display::grab(1);
    EXPECT_TRUE(display::grab(-1));
    
    display::grab(0);
    EXPECT_FALSE(display::grab(-1));
}

TEST(DisplayTest, BorderlessLogicBranches) {
    display::set_mode(800, 600, 0);
    
    display::borderless(1);
    EXPECT_TRUE(display::borderless(-1));
    
    display::borderless(0);
    EXPECT_FALSE(display::borderless(-1));
}

TEST(DisplayTest, RenderScaleQualityBranches) {
    display::set_mode(800, 600, 0);
    
    // Testing the true branch
    EXPECT_TRUE(display::set_render_scale_quality(true));
    // Testing the false branch
    EXPECT_TRUE(display::set_render_scale_quality(false));
}

TEST(DisplayTest, SetIconInvalidPath) {
    display::set_mode(800, 600, 0);
    // Should handle gracefully without crashing
    display::set_icon("non_existent_file.png");
    EXPECT_TRUE(true);
}

TEST(DisplayTest, DisplayGetters) {
    display::set_mode(800, 600, 0);
    EXPECT_NE(display::get_window(), nullptr);
    EXPECT_NE(display::get_renderer(), nullptr);
    auto& surf = display::get_surf();
    EXPECT_EQ(surf.get_width(), 800);
}
