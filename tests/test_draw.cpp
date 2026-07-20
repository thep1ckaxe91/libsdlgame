#include <gtest/gtest.h>
#include "draw.hpp"
#include "surface.hpp"
#include "color.hpp"
#include "rect.hpp"
#include "math.hpp"

class DrawTest : public ::testing::Test {
protected:
    sdlgame::surface::Surface surf;

    void SetUp() override {
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
