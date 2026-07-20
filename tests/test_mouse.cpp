#include <gtest/gtest.h>
#include "mouse.hpp"
#include "math.hpp"

TEST(MouseTest, GetPos) {
    sdlgame::math::Vector2 pos = sdlgame::mouse::get_pos();
    SUCCEED();
}

TEST(MouseTest, GetPressed) {
    auto pressed = sdlgame::mouse::get_pressed();
    EXPECT_EQ(pressed.size(), 5);
}

TEST(MouseTest, GetRel) {
    sdlgame::math::Vector2 rel = sdlgame::mouse::get_rel();
    SUCCEED();
}

TEST(MouseTest, Visibility) {
    bool initial_visibility = sdlgame::mouse::get_visible();
    
    sdlgame::mouse::set_visible(!initial_visibility);
    EXPECT_EQ(sdlgame::mouse::get_visible(), !initial_visibility);
    
    sdlgame::mouse::set_visible(initial_visibility);
    EXPECT_EQ(sdlgame::mouse::get_visible(), initial_visibility);
}
