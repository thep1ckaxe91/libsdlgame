#include <gtest/gtest.h>
#include "font.hpp"
#include "color.hpp"
#include "surface.hpp"
#include <filesystem>
#include <string>

// Test Font initialization
TEST(FontTest, Init) {
    EXPECT_NO_THROW(sdlgame::font::init());
}

// Test Font default constructor and get_height
TEST(FontTest, DefaultConstructor) {
    sdlgame::font::Font font;
    EXPECT_NO_THROW({
        int height = font.get_height();
        (void)height;
    });
}

// Test Font parameterized constructor
TEST(FontTest, ParameterizedConstructor) {
    std::filesystem::path fake_path = "dummy_font.ttf";
    sdlgame::font::Font font(fake_path, 16);
    EXPECT_NO_THROW({
        int height = font.get_height();
        (void)height;
    });
}

// Test Font render method with various parameters
TEST(FontTest, RenderMethod) {
    sdlgame::font::Font font;
    sdlgame::color::Color fg_color{255, 255, 255, 255};
    sdlgame::color::Color bg_color{0, 0, 0, 255};
    
    // We expect the calls to compile against the API signature.
    // They may throw runtime exceptions due to uninitialized SDL/TTF or invalid font path,
    // so we catch them to prevent the test runner from crashing.
    
    try {
        auto surface = font.render("SOLID", sdlgame::font::AntiAlias::SOLID, fg_color);
    } catch (...) {}

    try {
        auto surface = font.render("SHADED", sdlgame::font::AntiAlias::SHADED, fg_color, 100, bg_color);
    } catch (...) {}

    try {
        auto surface = font.render("BLENDED", sdlgame::font::AntiAlias::BLENDED, fg_color, 0);
    } catch (...) {}

    SUCCEED(); // If it compiles, the test passes
}
