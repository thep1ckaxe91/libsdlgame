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

// White-box tests for error handling and internal branches
TEST(FontDeathTest, InvalidFontPath) {
  sdlgame::init();
  EXPECT_DEATH(
      { sdlgame::font::Font font("nonexistent_path_to_font.ttf", 16); },
      "FATAL: SDL Error");
}

TEST(FontDeathTest, RenderWithUninitializedFont) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);
  sdlgame::font::Font uninitialized_font; // m_font is null
  sdlgame::color::Color fg_color{255, 255, 255, 255};
  
  EXPECT_DEATH(
      { uninitialized_font.render("Text", sdlgame::font::AntiAlias::SOLID, fg_color); },
      "FATAL: SDL Error");
  EXPECT_DEATH(
      { uninitialized_font.render("Text", sdlgame::font::AntiAlias::SHADED, fg_color); },
      "FATAL: SDL Error");
  EXPECT_DEATH(
      { uninitialized_font.render("Text", sdlgame::font::AntiAlias::BLENDED, fg_color); },
      "FATAL: SDL Error");
}

TEST(FontDeathTest, RenderEmptyTextSolid) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);
  sdlgame::font::Font font{std::filesystem::path("assets") / "dummy_font.ttf"};
  sdlgame::color::Color fg_color{255, 255, 255, 255};
  
  EXPECT_DEATH(
      { font.render("", sdlgame::font::AntiAlias::SOLID, fg_color); },
      "FATAL: SDL Error");
}

TEST(FontDeathTest, RenderEmptyTextShaded) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);
  sdlgame::font::Font font{std::filesystem::path("assets") / "dummy_font.ttf"};
  sdlgame::color::Color fg_color{255, 255, 255, 255};
  
  EXPECT_DEATH(
      { font.render("", sdlgame::font::AntiAlias::SHADED, fg_color); },
      "FATAL: SDL Error");
}

TEST(FontDeathTest, RenderEmptyTextBlended) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);
  sdlgame::font::Font font{std::filesystem::path("assets") / "dummy_font.ttf"};
  sdlgame::color::Color fg_color{255, 255, 255, 255};
  
  EXPECT_DEATH(
      { font.render("", sdlgame::font::AntiAlias::BLENDED, fg_color); },
      "FATAL: SDL Error");
}

TEST(FontTest, GetHeightReturnsCorrectSize) {
  sdlgame::init();
  std::filesystem::path fake_path =
      std::filesystem::current_path() / "assets/dummy_font.ttf";
  sdlgame::font::Font font1(fake_path, 24);
  EXPECT_EQ(font1.get_height(), 24);
  
  sdlgame::font::Font font2(fake_path, 48);
  EXPECT_EQ(font2.get_height(), 48);
}

TEST(FontTest, RenderMethodWrapLengthAndBackground) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);
  sdlgame::font::Font font{std::filesystem::path("assets") / "dummy_font.ttf"};
  sdlgame::color::Color fg_color{255, 255, 255, 255};
  sdlgame::color::Color bg_color{255, 0, 0, 255};
  
  // Test all AntiAlias paths to ensure background filling works correctly
  auto surface_solid = font.render(
      "Test Wrap SOLID", sdlgame::font::AntiAlias::SOLID, fg_color, 50, bg_color);
  EXPECT_GT(surface_solid.get_height(), 0);

  auto surface_shaded = font.render(
      "Test Wrap SHADED", sdlgame::font::AntiAlias::SHADED, fg_color, 50, bg_color);
  EXPECT_GT(surface_shaded.get_height(), 0);

  auto surface_blended = font.render(
      "Test Wrap BLENDED", sdlgame::font::AntiAlias::BLENDED, fg_color, 50, bg_color);
  EXPECT_GT(surface_blended.get_height(), 0);
}

TEST(FontTest, RenderMethodNewline) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);
  sdlgame::font::Font font{std::filesystem::path("assets") / "dummy_font.ttf"};
  sdlgame::color::Color fg_color{255, 255, 255, 255};
  
  auto surface_newline = font.render("Line1\nLine2", sdlgame::font::AntiAlias::SOLID, fg_color);
  EXPECT_GT(surface_newline.get_height(), font.get_height());
}
