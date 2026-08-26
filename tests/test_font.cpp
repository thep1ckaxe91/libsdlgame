#include "color.hpp"
#include "display.hpp"
#include "engine.hpp"
#include "font.hpp"
#include "surface.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <system_error>

// Test Font initialization
TEST(FontTest, Init) { EXPECT_NO_THROW(sdlgame::font::init()); }

// Test Font default constructor and get_height
TEST(FontTest, DefaultConstructor) {
  sdlgame::init();
  sdlgame::font::Font font;
  EXPECT_NO_THROW({
    int height = font.get_height();
    (void)height;
  });
}

// Test Font parameterized constructor
TEST(FontTest, ParameterizedConstructor) {
  sdlgame::init();
  std::error_code ec;

  std::filesystem::path fake_path =
      std::filesystem::current_path(ec) / "assets/dummy_font.ttf";

  EXPECT_FALSE(bool(ec));

  sdlgame::font::Font font(fake_path, 16);
  EXPECT_NO_THROW({
    int height = font.get_height();
    (void)height;
  });
}

// Test Font render method with various parameters
TEST(FontTest, RenderMethod) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);
  sdlgame::font::Font font{fs::path("assets") / "dummy_font.ttf"};
  sdlgame::color::Color fg_color{255, 255, 255, 255};
  sdlgame::color::Color bg_color{0, 0, 0, 255};

  {
    auto surface =
        font.render("SOLID", sdlgame::font::AntiAlias::SOLID, fg_color);
  }
  {
    auto surface = font.render("SHADED", sdlgame::font::AntiAlias::SHADED,
                               fg_color, 100, bg_color);
  }
  {
    auto surface =
        font.render("BLENDED", sdlgame::font::AntiAlias::BLENDED, fg_color, 0);
  }

  SUCCEED(); // If it compiles, the test passes
}
