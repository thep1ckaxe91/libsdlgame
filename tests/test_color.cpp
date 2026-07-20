#include <gtest/gtest.h>
#include "color.hpp"

using namespace sdlgame::color;

TEST(ColorTest, ConstructorsAndProperties) {
    Color default_color;
    // Just verifying compilation for default constructor
    (void)default_color;

    Color rgb_color(100, 150, 200);
    EXPECT_EQ(rgb_color.r, 100);
    EXPECT_EQ(rgb_color.g, 150);
    EXPECT_EQ(rgb_color.b, 200);
    EXPECT_EQ(rgb_color.a, 255);

    Color rgba_color(50, 100, 150, 200);
    EXPECT_EQ(rgba_color.r, 50);
    EXPECT_EQ(rgba_color.g, 100);
    EXPECT_EQ(rgba_color.b, 150);
    EXPECT_EQ(rgba_color.a, 200);

    Color named_color("red");
    (void)named_color;
}

TEST(ColorTest, MemberFunctions) {
    Color c(10, 20, 30, 40);
    
    SDL_Color sdl_c = c.to_SDL_Color();
    EXPECT_EQ(sdl_c.r, c.r);
    EXPECT_EQ(sdl_c.g, c.g);
    EXPECT_EQ(sdl_c.b, c.b);
    EXPECT_EQ(sdl_c.a, c.a);
    
    uint32_t uint_c = c.toUint32Color();
    (void)uint_c;
    
    std::string str_c = c.toString();
    (void)str_c;
}

TEST(ColorTest, Operators) {
    Color c1(100, 150, 200, 250);
    Color c2(10, 20, 30, 40);

    Color add_res = c1 + c2;
    Color sub_res = c1 - c2;
    Color mul_res = c1 * c2;
    Color div_res = c1 / c2;
    Color mod_res = c1 % c2;
    Color inv_res = ~c1;
    
    (void)add_res;
    (void)sub_res;
    (void)mul_res;
    (void)div_res;
    (void)mod_res;
    (void)inv_res;
}

TEST(ColorTest, Initialization) {
    // Calling the namespace function
    EXPECT_NO_THROW({
        sdlgame::color::init();
    });
}
