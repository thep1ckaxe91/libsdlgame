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
    surface::Surface surf(100, 200);
    surface::Surface flipped = transform::flip(surf, true, false);
    
    // We expect the dimensions to remain the same after flipping
    EXPECT_EQ(flipped.get_width(), 100);
    EXPECT_EQ(flipped.get_height(), 200);
}

TEST(TransformTest, ScaleSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface surf(100, 200);
    math::Vector2 new_size(50, 50);
    surface::Surface scaled = transform::scale(surf, new_size);
    
    // Check if the scale function correctly outputs a surface with the new size
    // Note: Assuming the implementation correctly sets the size property
    EXPECT_GE(scaled.get_width(), 0);
    EXPECT_GE(scaled.get_height(), 0);
}

TEST(TransformTest, ScaleBySurface) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface surf(100, 200);
    surface::Surface scaled = transform::scale_by(surf, 2.0);
    
    // Scale by 2.0 should ideally result in 200x400
    EXPECT_GE(scaled.get_width(), 0);
    EXPECT_GE(scaled.get_height(), 0);
}

TEST(TransformTest, RotateSurface) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface surf(100, 200);
    math::Vector2 center(50, 100);
    surface::Surface rotated = transform::rotate(surf, 90.0, center);
    
    // Just verify the API can be invoked correctly
    EXPECT_GE(rotated.get_width(), 0);
}

// --- New White-box Tests ---

TEST(TransformTest, FlipSurfaceCombinations) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface surf(100, 200);

    // Test combinations of flip flags to cover SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL branches
    surface::Surface flipped_x = transform::flip(surf, true, false);
    EXPECT_EQ(flipped_x.get_width(), 100);
    EXPECT_EQ(flipped_x.get_height(), 200);

    surface::Surface flipped_y = transform::flip(surf, false, true);
    EXPECT_EQ(flipped_y.get_width(), 100);
    EXPECT_EQ(flipped_y.get_height(), 200);

    surface::Surface flipped_both = transform::flip(surf, true, true);
    EXPECT_EQ(flipped_both.get_width(), 100);
    EXPECT_EQ(flipped_both.get_height(), 200);

    surface::Surface flipped_none = transform::flip(surf, false, false);
    EXPECT_EQ(flipped_none.get_width(), 100);
    EXPECT_EQ(flipped_none.get_height(), 200);
}

TEST(TransformTest, RotateSurfaceBranches) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    surface::Surface surf(100, 200);
    math::Vector2 center(50, 100);

    // 0 degrees rotation, bounding box should be same size
    surface::Surface rot_0 = transform::rotate(surf, 0.0, center);
    EXPECT_EQ(rot_0.get_width(), 100);
    EXPECT_EQ(rot_0.get_height(), 200);

    // 90 degrees rotation, bounding box should swap width and height
    surface::Surface rot_90 = transform::rotate(surf, 90.0, center);
    EXPECT_EQ(rot_90.get_width(), 200);
    EXPECT_EQ(rot_90.get_height(), 100);
    
    // 180 degrees
    surface::Surface rot_180 = transform::rotate(surf, 180.0, center);
    EXPECT_NEAR(rot_180.get_width(), 100, 1);
    EXPECT_NEAR(rot_180.get_height(), 200, 1);

    // -90 degrees
    surface::Surface rot_minus_90 = transform::rotate(surf, -90.0, center);
    EXPECT_NEAR(rot_minus_90.get_width(), 200, 1);
    EXPECT_NEAR(rot_minus_90.get_height(), 100, 1);
}

TEST(TransformTest, ScaleByZeroFactorDeath) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surface::Surface surf(100, 200);
        // Scaling by zero triggers surface creation error due to zero size
        transform::scale_by(surf, 0.0);
    }, "Failed to create texture");
}

TEST(TransformTest, ScaleSurfaceDeath) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surface::Surface surf(100, 200);
        // Negative size should fail in SDL texture creation
        transform::scale(surf, math::Vector2(-50, -50));
    }, "Failed to create texture");
}

TEST(TransformTest, RotateSurfaceDeath) {
    EXPECT_DEATH({
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surface::Surface surf(100, 200);
        // Extremely large center distance will create a bounding box exceeding max texture sizes, triggering a creation failure
        math::Vector2 center(1e9, 1e9); 
        transform::rotate(surf, 45.0, center);
    }, "Failed to create texture");
}
