#include <gtest/gtest.h>
#include "display.hpp"
#include "engine.hpp"
#include "surface.hpp"
#include "color.hpp"
#include "math.hpp"
#include "rect.hpp"
#include <utility>

using namespace sdlgame::surface;

TEST(SurfaceTest, DimensionsConstructorAndGetters) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);

    Surface surf(800, 600);
    EXPECT_DOUBLE_EQ(surf.get_width(), 800.0);
    EXPECT_DOUBLE_EQ(surf.get_height(), 600.0);
    
    auto size = surf.get_size();
    EXPECT_DOUBLE_EQ(size.x, 800.0);
    EXPECT_DOUBLE_EQ(size.y, 600.0);
    
    auto r = surf.get_rect();
    EXPECT_DOUBLE_EQ(r.getWidth(), 800.0);
    EXPECT_DOUBLE_EQ(r.getHeight(), 600.0);
}

TEST(SurfaceTest, FillAndBlitAPI) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    Surface dest(400, 300);
    Surface src(100, 100);
    
    dest.fill(sdlgame::color::Color(255, 0, 0, 255));
    
    dest.blit(src, sdlgame::math::Vector2(50, 50));
    dest.blit(src, sdlgame::math::Vector2(10, 10), sdlgame::math::Vector2(20, 20), sdlgame::rect::Rect(0, 0, 10, 10));
    
    SUCCEED();
}

TEST(SurfaceTest, MoveSemantics) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    Surface surf1(200, 150);
    Surface surf2(std::move(surf1));
    
    EXPECT_DOUBLE_EQ(surf2.get_width(), 200.0);
    EXPECT_DOUBLE_EQ(surf2.get_height(), 150.0);
    
    Surface surf3;
    surf3 = std::move(surf2);
    EXPECT_DOUBLE_EQ(surf3.get_width(), 200.0);
    EXPECT_DOUBLE_EQ(surf3.get_height(), 150.0);
}

TEST(SurfaceTest, CopySemantics) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    Surface surf1(300, 200);
    Surface surf2(surf1);
    
    EXPECT_DOUBLE_EQ(surf2.get_width(), 300.0);
    EXPECT_DOUBLE_EQ(surf2.get_height(), 200.0);
    
    Surface surf3;
    surf3 = surf1;
    EXPECT_DOUBLE_EQ(surf3.get_width(), 300.0);
    EXPECT_DOUBLE_EQ(surf3.get_height(), 200.0);
}
