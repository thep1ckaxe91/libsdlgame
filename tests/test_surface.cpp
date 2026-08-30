#include <gtest/gtest.h>
#include <SDL.h>
#include "display.hpp"
#include "engine.hpp"
#include "surface.hpp"
#include "color.hpp"
#include "math.hpp"
#include "rect.hpp"
#include <utility>
#include <type_traits>
#include <concepts>

using namespace sdlgame::surface;

TEST(SurfaceTest, DimensionsConstructorAndGetters) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);

    Surface<SDL_TEXTUREACCESS_TARGET> surf(800, 600);
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
    Surface<SDL_TEXTUREACCESS_TARGET> dest(400, 300);
    Surface<SDL_TEXTUREACCESS_TARGET> src(100, 100);
    
    dest.fill(sdlgame::color::Color(255, 0, 0, 255));
    
    dest.blit(src, sdlgame::math::Vector2(50, 50));
    dest.blit(src, sdlgame::math::Vector2(10, 10), sdlgame::math::Vector2(20, 20), sdlgame::rect::Rect(0, 0, 10, 10));
    
    SUCCEED();
}

TEST(SurfaceTest, MoveSemantics) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    Surface<SDL_TEXTUREACCESS_TARGET> surf1(200, 150);
    Surface<SDL_TEXTUREACCESS_TARGET> surf2(std::move(surf1));
    
    EXPECT_DOUBLE_EQ(surf2.get_width(), 200.0);
    EXPECT_DOUBLE_EQ(surf2.get_height(), 150.0);
    
    Surface<SDL_TEXTUREACCESS_TARGET> surf3;
    surf3 = std::move(surf2);
    EXPECT_DOUBLE_EQ(surf3.get_width(), 200.0);
    EXPECT_DOUBLE_EQ(surf3.get_height(), 150.0);
}

TEST(SurfaceTest, CopySemantics) {
    // Copy semantics are explicitly deleted in the new API.
    // Testing that the compiler correctly prevents copying.
    EXPECT_FALSE(std::is_copy_constructible_v<Surface<SDL_TEXTUREACCESS_TARGET>>);
    EXPECT_FALSE(std::is_copy_assignable_v<Surface<SDL_TEXTUREACCESS_TARGET>>);
}

TEST(SurfaceTest, ConstructorFromSDLSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    SDL_Surface* sdl_surf = SDL_CreateRGBSurfaceWithFormat(0, 50, 50, 32, SDL_PIXELFORMAT_RGBA32);
    ASSERT_NE(sdl_surf, nullptr);
    
    Surface<SDL_TEXTUREACCESS_TARGET> surf(sdl_surf);
    EXPECT_DOUBLE_EQ(surf.get_width(), 50.0);
    EXPECT_DOUBLE_EQ(surf.get_height(), 50.0);
    
    SDL_FreeSurface(sdl_surf);
}

TEST(SurfaceTest, ConstructorFromSDLTexture) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    SDL_Texture* tex = SDL_CreateTexture(sdlgame::display::get_renderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 20, 20);
    ASSERT_NE(tex, nullptr);
    
    Surface<SDL_TEXTUREACCESS_TARGET> surf(tex);
    EXPECT_DOUBLE_EQ(surf.get_width(), 20.0);
    EXPECT_DOUBLE_EQ(surf.get_height(), 20.0);
    
    SDL_DestroyTexture(tex);
}

TEST(SurfaceTest, AssignmentOperatorNullTexture) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface<SDL_TEXTUREACCESS_TARGET> s1(10, 10);
    Surface<SDL_TEXTUREACCESS_TARGET> s2; // default constructed, texture is null, size is 0,0
    
    // Adapted to use move since copy is deleted
    s1 = std::move(s2);
    EXPECT_EQ(s1.getTexture(), nullptr);
    EXPECT_DOUBLE_EQ(s1.get_width(), 0.0);
    EXPECT_DOUBLE_EQ(s1.get_height(), 0.0);
}

TEST(SurfaceTest, AssignmentOperatorSelfAssignment) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface<SDL_TEXTUREACCESS_TARGET> s1(10, 10);
    // Copy self-assignment is no longer possible since copy assignment is deleted.
    EXPECT_FALSE(std::is_copy_assignable_v<Surface<SDL_TEXTUREACCESS_TARGET>>);
}

TEST(SurfaceTest, MoveAssignmentSelfAssignment) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface<SDL_TEXTUREACCESS_TARGET> s1(15, 15);
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
    
    Surface<SDL_TEXTUREACCESS_TARGET> dest(100, 100);
    Surface<SDL_TEXTUREACCESS_TARGET> src(50, 50);
    
    // Negative size test: _size.x < 0, _size.y < 0
    dest.blit(src, sdlgame::math::Vector2(0, 0), sdlgame::math::Vector2(-5, -5), sdlgame::rect::Rect(0, 0, 10, 10));
    
    // area != rect::Rect() and negative size
    dest.blit(src, sdlgame::math::Vector2(0, 0), sdlgame::math::Vector2(20, -10), sdlgame::rect::Rect(10, 10, 20, 20));
    
    SUCCEED();
}

// --- New White-box Tests ---

TEST(SurfaceTest, LockIfNeededStreaming) {
    sdlgame::init();
    sdlgame::display::set_mode(800, 600);

    Surface<SDL_TEXTUREACCESS_STREAMING> surf(100, 100);
    // Should execute the streaming branch in lock()
    surf.lock(); 
    SUCCEED();
}



TEST(SurfaceTest, BlitDifferentAccessPatterns) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface<SDL_TEXTUREACCESS_TARGET> dest(400, 300);
    Surface src_static(100, 100);
    Surface<SDL_TEXTUREACCESS_STREAMING> src_streaming(100, 100);
    
    // Test that the templated blit correctly interoperates with other surface types
    dest.blit(src_static, sdlgame::math::Vector2(0, 0));
    dest.blit(src_streaming, sdlgame::math::Vector2(100, 100));
    
    SUCCEED();
}

TEST(SurfaceTest, ConstructorFromSDLTextureInvalidArgument) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    SDL_Texture* tex = SDL_CreateTexture(sdlgame::display::get_renderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 20, 20);
    ASSERT_NE(tex, nullptr);
    
    // Passing a TARGET texture to a STREAMING surface should throw
    EXPECT_THROW(Surface<SDL_TEXTUREACCESS_STREAMING> surf(tex), std::invalid_argument);
    
    SDL_DestroyTexture(tex);
}

TEST(SurfaceTest, FillStreamingSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface<SDL_TEXTUREACCESS_STREAMING> surf(10, 10);
    surf.fill(sdlgame::color::Color(255, 0, 0, 255)); // Should use the streaming fill logic
    
    SUCCEED();
}

TEST(SurfaceTest, LockUnlockStreaming) {
    sdlgame::init();
    sdlgame::display::set_mode(800,600);
    
    Surface<SDL_TEXTUREACCESS_STREAMING> surf(10, 10);
    surf.lock();
    auto [pixels, pitch] = surf.get_lock();
    EXPECT_NE(pixels, nullptr);
    EXPECT_GT(pitch, 0);
    surf.unlock();
    
    SUCCEED();
}

template <typename T>
concept HasLock = requires(T t) { t.lock(); };

TEST(SurfaceTest, LockRequiresStreaming) {
    EXPECT_TRUE(HasLock<Surface<SDL_TEXTUREACCESS_STREAMING>>);
    EXPECT_FALSE(HasLock<Surface<SDL_TEXTUREACCESS_TARGET>>);
    EXPECT_FALSE(HasLock<Surface<SDL_TEXTUREACCESS_STATIC>>);
}

TEST(SurfaceTest, CopyBetweenAccessPatterns) {
    sdlgame::init();
    sdlgame::display::set_mode(800, 600);

    Surface<SDL_TEXTUREACCESS_TARGET> target(100, 100);
    target.fill(sdlgame::color::Color(255, 0, 0, 255));

    auto streaming = target.copy<SDL_TEXTUREACCESS_STREAMING>();
    EXPECT_EQ(streaming.get_width(), 100);
    EXPECT_EQ(streaming.get_height(), 100);
    
    auto static_surf = streaming.copy<SDL_TEXTUREACCESS_STATIC>();
    EXPECT_EQ(static_surf.get_width(), 100);
    EXPECT_EQ(static_surf.get_height(), 100);
    
    auto target2 = static_surf.copy<SDL_TEXTUREACCESS_TARGET>();
    EXPECT_EQ(target2.get_width(), 100);
    EXPECT_EQ(target2.get_height(), 100);
}
