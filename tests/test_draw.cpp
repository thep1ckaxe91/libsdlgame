#include <gtest/gtest.h>
#include "display.hpp"
#include "draw.hpp"
#include "engine.hpp"
#include "surface.hpp"
#include "color.hpp"
#include "rect.hpp"
#include "math.hpp"

class DrawTest : public ::testing::Test {
protected:
    sdlgame::surface::Surface surf;

    void SetUp() override {
        sdlgame::init();
        sdlgame::display::set_mode(600,400);
        surf = sdlgame::surface::Surface(100, 100);
    }
};

TEST_F(DrawTest, DrawRect) {
    sdlgame::color::Color color(255, 0, 0, 255);
    sdlgame::rect::Rect rect(10, 10, 50, 50);
    
    EXPECT_NO_THROW({
        sdlgame::draw::rect(surf, color, rect, 1);
    });
}

TEST_F(DrawTest, DrawLine) {
    sdlgame::color::Color color(0, 255, 0, 255);
    
    EXPECT_NO_THROW({
        sdlgame::draw::line(surf, color, 0.0, 0.0, 100.0, 100.0);
    });
    
    sdlgame::math::Vector2 start(0.0, 0.0);
    sdlgame::math::Vector2 end(100.0, 100.0);
    
    EXPECT_NO_THROW({
        sdlgame::draw::line(surf, color, start, end);
    });
}

TEST_F(DrawTest, DrawCircle) {
    sdlgame::color::Color color(255, 255, 0, 255);
    
    EXPECT_NO_THROW({
        sdlgame::draw::circle(surf, color, 50, 50, 20, 2);
    });
}

TEST_F(DrawTest, DrawPolygon) {
    sdlgame::color::Color color(0, 255, 255, 255);
    std::vector<sdlgame::math::Vector2> pts = {
        sdlgame::math::Vector2(10.0, 10.0),
        sdlgame::math::Vector2(20.0, 10.0),
        sdlgame::math::Vector2(15.0, 20.0)
    };
    
    EXPECT_NO_THROW({
        sdlgame::draw::polygon(surf, color, pts);
    });
}

TEST_F(DrawTest, DrawPointAndPoints) {
    sdlgame::color::Color color(255, 0, 255, 255);
    
    EXPECT_NO_THROW({
        sdlgame::draw::point(surf, color, 50.0, 50.0);
    });
    
    std::vector<sdlgame::math::Vector2> pts = {
        sdlgame::math::Vector2(10.0, 10.0),
        sdlgame::math::Vector2(20.0, 20.0)
    };
    EXPECT_NO_THROW({
        sdlgame::draw::points(surf, color, pts);
    });
}

TEST_F(DrawTest, DrawRectWidthZero) {
    sdlgame::color::Color color(255, 0, 0, 255);
    sdlgame::rect::Rect rect(10, 10, 50, 50);
    
    // Covers width == 0 branch (filled rect)
    EXPECT_NO_THROW({
        sdlgame::draw::rect(surf, color, rect, 0);
    });
}

TEST_F(DrawTest, DrawRectWidthNegative) {
    sdlgame::color::Color color(255, 0, 0, 255);
    sdlgame::rect::Rect rect(10, 10, 50, 50);
    
    // Covers width < 0 branch (skips filling completely)
    EXPECT_NO_THROW({
        sdlgame::draw::rect(surf, color, rect, -1);
    });
}

TEST_F(DrawTest, DrawCircleWidthZero) {
    sdlgame::color::Color color(255, 255, 0, 255);
    
    // Covers width == 0 branch (filled circle)
    EXPECT_NO_THROW({
        sdlgame::draw::circle(surf, color, 50, 50, 20, 0);
    });
}

TEST_F(DrawTest, DrawCircleRadiusNegative) {
    sdlgame::color::Color color(255, 255, 0, 255);
    
    // Covers radius < 0 branch for filled circle (loop condition failure)
    EXPECT_NO_THROW({
        sdlgame::draw::circle(surf, color, 50, 50, -5, 0);
    });
}

TEST_F(DrawTest, DrawPolygonInvalidSize) {
    sdlgame::color::Color color(0, 255, 255, 255);
    std::vector<sdlgame::math::Vector2> two_pts = {
        sdlgame::math::Vector2(10.0, 10.0),
        sdlgame::math::Vector2(20.0, 10.0)
    };
    
    // Covers points.size() < 3 error branch
    EXPECT_THROW({
        sdlgame::draw::polygon(surf, color, two_pts);
    }, std::invalid_argument);

    std::vector<sdlgame::math::Vector2> zero_pts;
    EXPECT_THROW({
        sdlgame::draw::polygon(surf, color, zero_pts);
    }, std::invalid_argument);
}

TEST_F(DrawTest, DrawPointsEmpty) {
    sdlgame::color::Color color(255, 0, 255, 255);
    std::vector<sdlgame::math::Vector2> empty_pts;
    
    // Covers points.empty() case
    EXPECT_NO_THROW({
        sdlgame::draw::points(surf, color, empty_pts);
    });
}
