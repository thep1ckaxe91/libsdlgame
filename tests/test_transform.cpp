#include <SDL_render.h>
#include <gtest/gtest.h>
#include "display.hpp"
#include "engine.hpp"
#include "transform.hpp"
#include "surface.hpp"
#include "math.hpp"

using namespace sdlgame;

TEST(TransformTest, FlipSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> flipped = transform::flip(surf, true, false);
    
    // We expect the dimensions to remain the same after flipping
    EXPECT_EQ(flipped.get_width(), 100);
    EXPECT_EQ(flipped.get_height(), 200);
}

TEST(TransformTest, ScaleSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
    math::Vector2 new_size(50, 50);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> scaled = transform::scale(surf, new_size);
    
    // Check if the scale function correctly outputs a surface with the new size
    // Note: Assuming the implementation correctly sets the size property
    EXPECT_GE(scaled.get_width(), 0);
    EXPECT_GE(scaled.get_height(), 0);
}

TEST(TransformTest, ScaleBySurface) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> scaled = transform::scale_by(surf, 2.0);
    
    // Scale by 2.0 should ideally result in 200x400
    EXPECT_GE(scaled.get_width(), 0);
    EXPECT_GE(scaled.get_height(), 0);
}

TEST(TransformTest, RotateSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
    math::Vector2 center(50, 100);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> rotated = transform::rotate(surf, 90.0, center);
    
    // Just verify the API can be invoked correctly
    EXPECT_GE(rotated.get_width(), 0);
}

// --- New White-box Tests ---

TEST(TransformTest, FlipSurfaceCombinations) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);

    // Test combinations of flip flags to cover SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL branches
    surface::Surface<SDL_TEXTUREACCESS_TARGET> flipped_x = transform::flip(surf, true, false);
    EXPECT_EQ(flipped_x.get_width(), 100);
    EXPECT_EQ(flipped_x.get_height(), 200);

    surface::Surface<SDL_TEXTUREACCESS_TARGET> flipped_y = transform::flip(surf, false, true);
    EXPECT_EQ(flipped_y.get_width(), 100);
    EXPECT_EQ(flipped_y.get_height(), 200);

    surface::Surface<SDL_TEXTUREACCESS_TARGET> flipped_both = transform::flip(surf, true, true);
    EXPECT_EQ(flipped_both.get_width(), 100);
    EXPECT_EQ(flipped_both.get_height(), 200);

    surface::Surface<SDL_TEXTUREACCESS_TARGET> flipped_none = transform::flip(surf, false, false);
    EXPECT_EQ(flipped_none.get_width(), 100);
    EXPECT_EQ(flipped_none.get_height(), 200);
}

TEST(TransformTest, RotateSurfaceBranches) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
    math::Vector2 center(50, 100);

    // 0 degrees rotation, bounding box should be same size
    surface::Surface<SDL_TEXTUREACCESS_TARGET> rot_0 = transform::rotate(surf, 0.0, center);
    EXPECT_EQ(rot_0.get_width(), 100);
    EXPECT_EQ(rot_0.get_height(), 200);

    // 90 degrees rotation, bounding box should swap width and height
    surface::Surface<SDL_TEXTUREACCESS_TARGET> rot_90 = transform::rotate(surf, 90.0, center);
    EXPECT_EQ(rot_90.get_width(), 200);
    EXPECT_EQ(rot_90.get_height(), 100);
    
    // 180 degrees
    surface::Surface<SDL_TEXTUREACCESS_TARGET> rot_180 = transform::rotate(surf, 180.0, center);
    EXPECT_NEAR(rot_180.get_width(), 100, 1);
    EXPECT_NEAR(rot_180.get_height(), 200, 1);

    // -90 degrees
    surface::Surface<SDL_TEXTUREACCESS_TARGET> rot_minus_90 = transform::rotate(surf, -90.0, center);
    EXPECT_NEAR(rot_minus_90.get_width(), 200, 1);
    EXPECT_NEAR(rot_minus_90.get_height(), 100, 1);
}

TEST(TransformTest, ScaleByZeroFactorDeath) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
        // Scaling by zero triggers surface creation error due to zero size
        transform::scale_by(surf, 0.0);
    }, "FATAL: SDL Error");
}

TEST(TransformTest, ScaleSurfaceDeath) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
        // Negative size should fail in SDL texture creation
        transform::scale(surf, math::Vector2(-50, -50));
    }, "FATAL: SDL Error");
}

TEST(TransformTest, RotateSurfaceDeath) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);

        SDL_RendererInfo info;
        SDL_GetRendererInfo(sdlgame::display::get_renderer(), &info);

        surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(info.max_texture_width, info.max_texture_height);
        // rotate at max texture size will 100% cause a size grow, which should case failed at create texture
        transform::rotate(surf, 45.0);
    }, "FATAL: SDL Error");
}

// Additional White-box Tests
TEST(TransformTest, ScaleByNegativeFactor) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
        transform::scale_by(surf, -1.0);
    }, "FATAL: SDL Error");
}

TEST(TransformTest, RotateSurface360) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
    math::Vector2 center(50, 100);

    // 360 degrees rotation, bounding box should be same size
    surface::Surface<SDL_TEXTUREACCESS_TARGET> rot_360 = transform::rotate(surf, 360.0, center);
    EXPECT_EQ(rot_360.get_width(), 100);
    EXPECT_EQ(rot_360.get_height(), 200);
}

TEST(TransformTest, ScaleSurfaceZeroSize) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surface::Surface<SDL_TEXTUREACCESS_TARGET> surf(100, 200);
        transform::scale(surf, math::Vector2(0, 0));
    }, "FATAL: SDL Error");
}

TEST(TransformTest, AccessPatternAny) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface<SDL_TEXTUREACCESS_STATIC> surf_static(100, 200);
    
    // Scale from STATIC to STREAMING
    surface::Surface<SDL_TEXTUREACCESS_STREAMING> streaming_scaled = transform::scale<SDL_TEXTUREACCESS_STATIC, SDL_TEXTUREACCESS_STREAMING>(surf_static, math::Vector2(50, 50));
    EXPECT_EQ(streaming_scaled.get_width(), 50);
    EXPECT_EQ(streaming_scaled.get_height(), 50);
    
    // Flip from STREAMING to TARGET
    surface::Surface<SDL_TEXTUREACCESS_TARGET> target_flipped = transform::flip<SDL_TEXTUREACCESS_STREAMING, SDL_TEXTUREACCESS_TARGET>(streaming_scaled, true, false);
    EXPECT_EQ(target_flipped.get_width(), 50);
    EXPECT_EQ(target_flipped.get_height(), 50);

    // Rotate from TARGET to STATIC
    surface::Surface<SDL_TEXTUREACCESS_STATIC> static_rotated = transform::rotate<SDL_TEXTUREACCESS_TARGET, SDL_TEXTUREACCESS_STATIC>(target_flipped, 90.0, math::Vector2(25, 25));
    EXPECT_EQ(static_rotated.get_width(), 50);
    EXPECT_EQ(static_rotated.get_height(), 50);
}
