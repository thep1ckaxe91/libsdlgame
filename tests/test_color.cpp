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

    // Keep black-box testing from previous code
    Color named_color("red");
    EXPECT_EQ(named_color.r, 255);
    EXPECT_EQ(named_color.g, 0);
    EXPECT_EQ(named_color.b, 0);
    EXPECT_EQ(named_color.a, 255);
}

TEST(ColorTest, TemplateConstructors) {
    // White-box test for template constructors using types convertible to uint8_t
    char r_val = 10;
    short g_val = 20;
    int b_val = 30;
    long a_val = 40;
    
    Color c_mixed(r_val, g_val, b_val, a_val);
    EXPECT_EQ(c_mixed.r, 10);
    EXPECT_EQ(c_mixed.g, 20);
    EXPECT_EQ(c_mixed.b, 30);
    EXPECT_EQ(c_mixed.a, 40);

    Color c_mixed_3(r_val, g_val, b_val);
    EXPECT_EQ(c_mixed_3.r, 10);
    EXPECT_EQ(c_mixed_3.g, 20);
    EXPECT_EQ(c_mixed_3.b, 30);
    EXPECT_EQ(c_mixed_3.a, 255);

    // Testing constexpr evaluation
    constexpr Color c_constexpr(1, 2, 3, 255);
    static_assert(c_constexpr.r == 1 && c_constexpr.g == 2 && c_constexpr.b == 3 && c_constexpr.a == 255);
    
    constexpr Color c_constexpr4(4, 5, 6, 7);
    static_assert(c_constexpr4.r == 4 && c_constexpr4.g == 5 && c_constexpr4.b == 6 && c_constexpr4.a == 7);
}

TEST(ColorTest, StringConstructorLogic) {
    // Mixed case handling
    Color mixed_red("ReD");
    EXPECT_EQ(mixed_red.r, 255);
    EXPECT_EQ(mixed_red.g, 0);
    EXPECT_EQ(mixed_red.b, 0);
    EXPECT_EQ(mixed_red.a, 255);

    // "none" handling
    Color none_color("none");
    EXPECT_EQ(none_color.r, 0);
    EXPECT_EQ(none_color.g, 0);
    EXPECT_EQ(none_color.b, 0);
    EXPECT_EQ(none_color.a, 0);

    // Empty string handling
    Color empty_color("");
    EXPECT_EQ(empty_color.r, 0);
    EXPECT_EQ(empty_color.g, 0);
    EXPECT_EQ(empty_color.b, 0);
    EXPECT_EQ(empty_color.a, 0);
    
    // Exact match for edge elements in __named_color array
    Color first_color("alice blue");
    EXPECT_EQ(first_color.r, 240);
    EXPECT_EQ(first_color.g, 248);
    EXPECT_EQ(first_color.b, 255);

    Color last_color("yellow green");
    EXPECT_EQ(last_color.r, 154);
    EXPECT_EQ(last_color.g, 205);
    EXPECT_EQ(last_color.b, 50);

    // Death tests for unknown colors, exploring lower_bound edge cases
    EXPECT_DEATH(Color("unknown_color_name"), "Unrecognize color identifier: unknown_color_name");
    EXPECT_DEATH(Color("a"), "Unrecognize color identifier: a"); // Before "alice blue"
    EXPECT_DEATH(Color("z"), "Unrecognize color identifier: z"); // After "yellow green"
    EXPECT_DEATH(Color("alica blue"), "Unrecognize color identifier: alica blue"); // Close mismatch
}

TEST(ColorTest, MemberFunctions) {
    Color c(10, 20, 30, 40);
    
    SDL_Color sdl_c = c.to_SDL_Color();
    EXPECT_EQ(sdl_c.r, 10);
    EXPECT_EQ(sdl_c.g, 20);
    EXPECT_EQ(sdl_c.b, 30);
    EXPECT_EQ(sdl_c.a, 40);
    
    // Default format RGBA32
    uint32_t uint_c = c.toUint32Color();
    EXPECT_NE(uint_c, 0u);
    
    // Testing specific format considering surface.hpp might use different optimal_format()
    uint32_t argb_c = c.toUint32Color(SDL_PIXELFORMAT_ARGB8888);
    EXPECT_NE(argb_c, 0u);
    EXPECT_NE(argb_c, uint_c); // ARGB8888 will likely have a different int value than RGBA32
    
    // White-box test for SDL_NEW error handling on SDL_AllocFormat
    EXPECT_DEATH(c.toUint32Color(SDL_PIXELFORMAT_UNKNOWN), "Failing to Create Resource at: SDL_AllocFormat");
    
    std::string str_c = c.toString();
    EXPECT_EQ(str_c, "Color(10,20,30,40)");
}

TEST(ColorTest, Operators) {
    Color c1(100, 150, 200, 250);
    Color c2(10, 20, 30, 40);

    Color add_res = c1 + c2;
    EXPECT_EQ(add_res.r, 110);
    EXPECT_EQ(add_res.g, 170);
    EXPECT_EQ(add_res.b, 230);
    EXPECT_EQ(add_res.a, 34); // 250 + 40 = 290; 290 % 256 = 34

    Color sub_res = c1 - c2;
    EXPECT_EQ(sub_res.r, 90);
    EXPECT_EQ(sub_res.g, 130);
    EXPECT_EQ(sub_res.b, 170);
    EXPECT_EQ(sub_res.a, 210);

    Color mul_res = c1 * c2;
    EXPECT_EQ(mul_res.r, static_cast<uint8_t>(100 * 10));
    EXPECT_EQ(mul_res.g, static_cast<uint8_t>(150 * 20));
    EXPECT_EQ(mul_res.b, static_cast<uint8_t>(200 * 30));
    EXPECT_EQ(mul_res.a, static_cast<uint8_t>(250 * 40));

    Color div_res = c1 / c2;
    EXPECT_EQ(div_res.r, 100 / 10);
    EXPECT_EQ(div_res.g, 150 / 20);
    EXPECT_EQ(div_res.b, 200 / 30);
    EXPECT_EQ(div_res.a, 250 / 40);

    Color mod_res = c1 % c2;
    EXPECT_EQ(mod_res.r, 100 % 10);
    EXPECT_EQ(mod_res.g, 150 % 20);
    EXPECT_EQ(mod_res.b, 200 % 30);
    EXPECT_EQ(mod_res.a, 250 % 40);

    Color inv_res = ~c1;
    EXPECT_EQ(inv_res.r, 255 - 100);
    EXPECT_EQ(inv_res.g, 255 - 150);
    EXPECT_EQ(inv_res.b, 255 - 200);
    EXPECT_EQ(inv_res.a, 250); // Alpha does not get inverted
}
