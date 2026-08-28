#include <gtest/gtest.h>
#include "rect.hpp"

TEST(RectTest, BasicInitialization) {
    sdlgame::rect::Rect r(10.0, 20.0, 30.0, 40.0);
    EXPECT_DOUBLE_EQ(r.getLeft(), 10.0);
    EXPECT_DOUBLE_EQ(r.getTop(), 20.0);
    EXPECT_DOUBLE_EQ(r.getWidth(), 30.0);
    EXPECT_DOUBLE_EQ(r.getHeight(), 40.0);
}

TEST(RectTest, Contains) {
    sdlgame::rect::Rect outer(0.0, 0.0, 100.0, 100.0);
    sdlgame::rect::Rect inner(10.0, 10.0, 50.0, 50.0);
    sdlgame::rect::Rect outside(150.0, 150.0, 10.0, 10.0);

    EXPECT_TRUE(outer.contains(inner));
    EXPECT_FALSE(outer.contains(outside));
}

TEST(RectTest, Overlap) {
    sdlgame::rect::Rect r1(0.0, 0.0, 100.0, 100.0);
    sdlgame::rect::Rect r2(50.0, 50.0, 100.0, 100.0);
    
    // Overlapping case
    sdlgame::rect::Rect overlap_rect = r1.overlap(r2);
    EXPECT_DOUBLE_EQ(overlap_rect.getLeft(), 50.0);
    EXPECT_DOUBLE_EQ(overlap_rect.getTop(), 50.0);
    EXPECT_DOUBLE_EQ(overlap_rect.getWidth(), 50.0);
    EXPECT_DOUBLE_EQ(overlap_rect.getHeight(), 50.0);

    // Non-overlapping case
    sdlgame::rect::Rect r3(200.0, 200.0, 50.0, 50.0);
    sdlgame::rect::Rect no_overlap = r1.overlap(r3);
    EXPECT_DOUBLE_EQ(no_overlap.getLeft(), 0.0);
    EXPECT_DOUBLE_EQ(no_overlap.getTop(), 0.0);
    EXPECT_DOUBLE_EQ(no_overlap.getWidth(), 0.0);
    EXPECT_DOUBLE_EQ(no_overlap.getHeight(), 0.0);
}

TEST(RectTest, Clipline) {
    sdlgame::rect::Rect r(10.0, 10.0, 80.0, 80.0);

    // 1. Line completely inside
    auto res = r.clipline(sdlgame::math::Vector2(20.0, 20.0), sdlgame::math::Vector2(70.0, 70.0));
    ASSERT_TRUE(res.has_value());
    EXPECT_DOUBLE_EQ(res->first.x, 20.0);
    EXPECT_DOUBLE_EQ(res->first.y, 20.0);
    EXPECT_DOUBLE_EQ(res->second.x, 70.0);
    EXPECT_DOUBLE_EQ(res->second.y, 70.0);

    // 2. Line intersects the rect
    auto res2 = r.clipline(sdlgame::math::Vector2(0.0, 50.0), sdlgame::math::Vector2(100.0, 50.0));
    ASSERT_TRUE(res2.has_value());
    EXPECT_DOUBLE_EQ(res2->first.x, 10.0); // clamped to left edge
    EXPECT_DOUBLE_EQ(res2->first.y, 50.0);
    EXPECT_DOUBLE_EQ(res2->second.x, 90.0); // clamped to right edge (10 + 80 = 90)
    EXPECT_DOUBLE_EQ(res2->second.y, 50.0);

    // 3. Line completely outside
    auto res3 = r.clipline(sdlgame::math::Vector2(0.0, 0.0), sdlgame::math::Vector2(0.0, 100.0));
    EXPECT_FALSE(res3.has_value());

    // 4. Line parallel and outside (c.values[i] == 0.0, q.values[i] < 0)
    auto res4 = r.clipline(sdlgame::math::Vector2(5.0, 0.0), sdlgame::math::Vector2(5.0, 100.0));
    EXPECT_FALSE(res4.has_value());
    
    // 5. Line rejected because t0 > t1 (enters after leaving)
    auto res5 = r.clipline(sdlgame::math::Vector2(0.0, 0.0), sdlgame::math::Vector2(100.0, 5.0));
    EXPECT_FALSE(res5.has_value());
    
    // 6. Line intersecting just one edge (starts inside, ends outside)
    auto res6 = r.clipline(sdlgame::math::Vector2(50.0, 50.0), sdlgame::math::Vector2(100.0, 100.0));
    ASSERT_TRUE(res6.has_value());
    EXPECT_DOUBLE_EQ(res6->first.x, 50.0);
    EXPECT_DOUBLE_EQ(res6->first.y, 50.0);
    EXPECT_DOUBLE_EQ(res6->second.x, 90.0);
    EXPECT_DOUBLE_EQ(res6->second.y, 90.0);
}

TEST(RectTest, Fit) {
    sdlgame::rect::Rect r(10.0, 10.0, 50.0, 100.0); // aspect ratio 0.5
    sdlgame::rect::Rect oth(0.0, 0.0, 20.0, 20.0);  // width 20
    
    sdlgame::rect::Rect fit_rect = r.fit(oth);
    EXPECT_DOUBLE_EQ(fit_rect.getLeft(), 10.0); // position kept
    EXPECT_DOUBLE_EQ(fit_rect.getTop(), 10.0);
    EXPECT_DOUBLE_EQ(fit_rect.getWidth(), 20.0); // 50 * (20 / 50) = 20
    EXPECT_DOUBLE_EQ(fit_rect.getHeight(), 40.0); // 100 * (20 / 50) = 40
}

TEST(RectTest, CollideList) {
    sdlgame::rect::Rect r(0.0, 0.0, 50.0, 50.0);
    std::vector<sdlgame::rect::Rect> list1 = {
        sdlgame::rect::Rect(100.0, 100.0, 10.0, 10.0),
        sdlgame::rect::Rect(20.0, 20.0, 10.0, 10.0)
    };
    EXPECT_TRUE(r.collidelist(list1));
    
    std::vector<sdlgame::rect::Rect> list2 = {
        sdlgame::rect::Rect(100.0, 100.0, 10.0, 10.0),
        sdlgame::rect::Rect(200.0, 200.0, 10.0, 10.0)
    };
    EXPECT_FALSE(r.collidelist(list2));
}

TEST(RectTest, Collisions) {
    sdlgame::rect::Rect r(10.0, 10.0, 40.0, 40.0); // right = 50, bottom = 50
    
    // collidepoint edges
    EXPECT_TRUE(r.collidepoint(10.0, 10.0));
    EXPECT_TRUE(r.collidepoint(50.0, 50.0));
    EXPECT_FALSE(r.collidepoint(9.9, 10.0));
    EXPECT_FALSE(r.collidepoint(50.1, 50.0));
    
    // colliderect boundaries
    sdlgame::rect::Rect touch_left(0.0, 10.0, 10.0, 40.0); // right = 10, touches left
    EXPECT_TRUE(r.colliderect(touch_left));
    
    sdlgame::rect::Rect out_left(0.0, 10.0, 9.9, 40.0);
    EXPECT_FALSE(r.colliderect(out_left));
}

TEST(RectTest, Inflate) {
    sdlgame::rect::Rect r(10.0, 10.0, 20.0, 20.0);
    
    sdlgame::rect::Rect r2 = r.inflate(10.0, 10.0);
    EXPECT_DOUBLE_EQ(r2.getLeft(), 5.0);
    EXPECT_DOUBLE_EQ(r2.getTop(), 5.0);
    EXPECT_DOUBLE_EQ(r2.getWidth(), 30.0);
    EXPECT_DOUBLE_EQ(r2.getHeight(), 30.0);
}

TEST(RectTest, ToSDLRect) {
    sdlgame::rect::Rect r(10.5, 20.5, 30.5, 40.5);
    
    SDL_FRect frect = r.to_SDL_FRect();
    EXPECT_FLOAT_EQ(frect.x, 10.5f);
    EXPECT_FLOAT_EQ(frect.y, 20.5f);
    EXPECT_FLOAT_EQ(frect.w, 30.5f);
    EXPECT_FLOAT_EQ(frect.h, 40.5f);
    
    SDL_Rect irect = r.to_SDL_Rect();
    EXPECT_EQ(irect.x, 10);
    EXPECT_EQ(irect.y, 20);
    EXPECT_EQ(irect.w, 30);
    EXPECT_EQ(irect.h, 40);
}
