#include <gtest/gtest.h>
#include <SDL.h>
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

TEST(SurfaceTest, ConstructorFromSDLSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    SDL_Surface* sdl_surf = SDL_CreateRGBSurfaceWithFormat(0, 50, 50, 32, SDL_PIXELFORMAT_RGBA32);
    ASSERT_NE(sdl_surf, nullptr);
    
    Surface surf(sdl_surf);
    EXPECT_DOUBLE_EQ(surf.get_width(), 50.0);
    EXPECT_DOUBLE_EQ(surf.get_height(), 50.0);
    
    SDL_FreeSurface(sdl_surf);
}

TEST(SurfaceTest, ConstructorFromSDLTexture) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    SDL_Texture* tex = SDL_CreateTexture(sdlgame::display::get_renderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 20, 20);
    ASSERT_NE(tex, nullptr);
    
    Surface surf(tex);
    EXPECT_DOUBLE_EQ(surf.get_width(), 20.0);
    EXPECT_DOUBLE_EQ(surf.get_height(), 20.0);
    
    SDL_DestroyTexture(tex);
}

TEST(SurfaceTest, AssignmentOperatorNullTexture) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface s1(10, 10);
    Surface s2; // default constructed, texture is null, size is 0,0
    
    s1 = s2;
    EXPECT_EQ(s1.getTexture(), nullptr);
    EXPECT_DOUBLE_EQ(s1.get_width(), 0.0);
    EXPECT_DOUBLE_EQ(s1.get_height(), 0.0);
}

TEST(SurfaceTest, AssignmentOperatorSelfAssignment) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface s1(10, 10);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif
    s1 = s1;
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
    
    EXPECT_NE(s1.getTexture(), nullptr);
    EXPECT_DOUBLE_EQ(s1.get_width(), 10.0);
    EXPECT_DOUBLE_EQ(s1.get_height(), 10.0);
}

TEST(SurfaceTest, MoveAssignmentSelfAssignment) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface s1(15, 15);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wself-move"
#endif
    s1 = std::move(s1);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
    
    EXPECT_NE(s1.getTexture(), nullptr);
    EXPECT_DOUBLE_EQ(s1.get_width(), 15.0);
    EXPECT_DOUBLE_EQ(s1.get_height(), 15.0);
}

TEST(SurfaceTest, BlitEdgeCases) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface dest(100, 100);
    Surface src(50, 50);
    
    // Negative size test: _size.x < 0, _size.y < 0
    dest.blit(src, sdlgame::math::Vector2(0, 0), sdlgame::math::Vector2(-5, -5), sdlgame::rect::Rect(0, 0, 10, 10));
    
    // area != rect::Rect() and negative size
    dest.blit(src, sdlgame::math::Vector2(0, 0), sdlgame::math::Vector2(20, -10), sdlgame::rect::Rect(10, 10, 20, 20));
    
    SUCCEED();
}
