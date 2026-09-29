#include <gtest/gtest.h>
#include "rect.hpp"
#include <cmath>


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
    auto res = r.clipline({20.0, 20.0}, {70.0, 70.0});
    ASSERT_TRUE(res.has_value());
    EXPECT_DOUBLE_EQ(res->first.x, 20.0);
    EXPECT_DOUBLE_EQ(res->first.y, 20.0);
    EXPECT_DOUBLE_EQ(res->second.x, 70.0);
    EXPECT_DOUBLE_EQ(res->second.y, 70.0);

    // 2. Line intersects the rect
    auto res2 = r.clipline({0.0, 50.0}, {100.0, 50.0});
    ASSERT_TRUE(res2.has_value());
    EXPECT_DOUBLE_EQ(res2->first.x, 10.0); // clamped to left edge
    EXPECT_DOUBLE_EQ(res2->first.y, 50.0);
    EXPECT_DOUBLE_EQ(res2->second.x, 90.0); // clamped to right edge (10 + 80 = 90)
    EXPECT_DOUBLE_EQ(res2->second.y, 50.0);

    // 3. Line completely outside
    auto res3 = r.clipline({0.0, 0.0}, {0.0, 100.0});
    EXPECT_FALSE(res3.has_value());

    // 4. Line parallel and outside (c.values[i] == 0.0, q.values[i] < 0)
    auto res4 = r.clipline({5.0, 0.0}, {5.0, 100.0});
    EXPECT_FALSE(res4.has_value());
    
    // 5. Line rejected because t0 > t1 (enters after leaving)
    auto res5 = r.clipline({0.0, 0.0}, {100.0, 5.0});
    EXPECT_FALSE(res5.has_value());
    
    // 6. Line intersecting just one edge (starts inside, ends outside)
    auto res6 = r.clipline({50.0, 50.0}, {100.0, 100.0});
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

// --- WHITE-BOX TESTS ---

TEST(RectWhiteBoxTest, CliplineParallelInside) {
    sdlgame::rect::Rect r(10.0, 10.0, 80.0, 80.0);
    // Line parallel to y-axis (vertical) and completely inside x boundaries
    auto res = r.clipline({20.0, 5.0}, {20.0, 95.0});
    ASSERT_TRUE(res.has_value());
    EXPECT_DOUBLE_EQ(res->first.x, 20.0);
    EXPECT_DOUBLE_EQ(res->first.y, 10.0);
    EXPECT_DOUBLE_EQ(res->second.x, 20.0);
    EXPECT_DOUBLE_EQ(res->second.y, 90.0);
}

TEST(RectWhiteBoxTest, ConstructorsAndUpdates) {
    sdlgame::math::Vector2 pos(5.0, 15.0);
    sdlgame::math::Vector2 size(25.0, 35.0);

    sdlgame::rect::Rect r1(5.0, 15.0, size);
    EXPECT_DOUBLE_EQ(r1.getLeft(), 5.0);
    EXPECT_DOUBLE_EQ(r1.getWidth(), 25.0);

    sdlgame::rect::Rect r2(pos, 25.0, 35.0);
    EXPECT_DOUBLE_EQ(r2.getTop(), 15.0);

    sdlgame::rect::Rect r3(pos, size);
    EXPECT_TRUE(r1 == r3);

    r3.update(1.0, 2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(r3.getLeft(), 1.0);

    r3.update(2.0, 3.0, size);
    EXPECT_DOUBLE_EQ(r3.getLeft(), 2.0);

    r3.update(pos, 4.0, 5.0);
    EXPECT_DOUBLE_EQ(r3.getTop(), 15.0);

    r3.update(pos, size);
    EXPECT_TRUE(r1 == r3);
}

TEST(RectWhiteBoxTest, ToString) {
    sdlgame::rect::Rect r(1.0, 2.0, 3.0, 4.0);
    std::string s = r.toString();
    EXPECT_NE(s.find("Rect<"), std::string::npos);
}

TEST(RectWhiteBoxTest, MoveAndInflateVector2) {
    sdlgame::rect::Rect r(0.0, 0.0, 10.0, 10.0);
    sdlgame::math::Vector2 offset(5.0, 5.0);
    
    sdlgame::rect::Rect moved = r.move(offset);
    EXPECT_DOUBLE_EQ(moved.getLeft(), 5.0);
    
    r.move_ip(offset);
    EXPECT_DOUBLE_EQ(r.getLeft(), 5.0);

    sdlgame::rect::Rect inflated = r.inflate(offset);
    EXPECT_DOUBLE_EQ(inflated.getWidth(), 15.0);
    
    r.inflate_ip(offset);
    EXPECT_DOUBLE_EQ(r.getWidth(), 15.0);
}

TEST(RectWhiteBoxTest, FitEdgeCases) {
    sdlgame::rect::Rect r(0.0, 0.0, 50.0, 50.0);
    sdlgame::rect::Rect oth(10.0, 10.0, 0.0, 0.0); // zero width/height

    sdlgame::rect::Rect fit_rect = r.fit(oth);
    EXPECT_DOUBLE_EQ(fit_rect.getWidth(), 0.0);
    EXPECT_DOUBLE_EQ(fit_rect.getHeight(), 0.0);

    sdlgame::rect::Rect r_zero(0.0, 0.0, 0.0, 0.0);
    sdlgame::rect::Rect oth_normal(10.0, 10.0, 20.0, 20.0);
    sdlgame::rect::Rect fit_zero = r_zero.fit(oth_normal);
    EXPECT_TRUE(std::isnan(fit_zero.getWidth()) || std::isinf(fit_zero.getWidth()) || fit_zero.getWidth() == 0.0);
}

TEST(RectWhiteBoxTest, ContainsEdgeCases) {
    sdlgame::rect::Rect r(0.0, 0.0, 100.0, 100.0);
    EXPECT_TRUE(r.contains(r));
    
    sdlgame::rect::Rect partial(50.0, 50.0, 100.0, 100.0);
    EXPECT_FALSE(r.contains(partial));
    
    sdlgame::rect::Rect outside(-50.0, -50.0, 10.0, 10.0);
    EXPECT_FALSE(r.contains(outside));
}

TEST(RectWhiteBoxTest, CollidePointVector2) {
    sdlgame::rect::Rect r(10.0, 10.0, 40.0, 40.0);
    EXPECT_TRUE(r.collidepoint({20.0, 20.0}));
    EXPECT_FALSE(r.collidepoint({0.0, 0.0}));
}

TEST(RectWhiteBoxTest, CollideListEmpty) {
    sdlgame::rect::Rect r(10.0, 10.0, 40.0, 40.0);
    std::vector<sdlgame::rect::Rect> empty_list;
    EXPECT_FALSE(r.collidelist(empty_list));
}

TEST(RectWhiteBoxTest, OverlapIP) {
    sdlgame::rect::Rect r1(0.0, 0.0, 100.0, 100.0);
    sdlgame::rect::Rect r2(50.0, 50.0, 100.0, 100.0);
    r1.overlap_ip(r2);
    EXPECT_DOUBLE_EQ(r1.getLeft(), 50.0);
    EXPECT_DOUBLE_EQ(r1.getWidth(), 50.0);
}

TEST(RectWhiteBoxTest, GetterSetterConsistency) {
    sdlgame::rect::Rect r(10.0, 20.0, 30.0, 40.0);
    
    EXPECT_DOUBLE_EQ(r.getSize().x, r.getWidth());
    EXPECT_DOUBLE_EQ(r.getSize().y, r.getHeight());
    
    EXPECT_DOUBLE_EQ(r.getCenter().x, r.getCenterX());
    EXPECT_DOUBLE_EQ(r.getCenter().y, r.getCenterY());
    
    EXPECT_DOUBLE_EQ(r.getTopLeft().x, r.getLeft());
    EXPECT_DOUBLE_EQ(r.getTopLeft().y, r.getTop());
    
    r.setTopLeft({0.0, 0.0});
    EXPECT_DOUBLE_EQ(r.getLeft(), 0.0);
    EXPECT_DOUBLE_EQ(r.getTop(), 0.0);
    
    r.setBottomRight({100.0, 100.0});
    EXPECT_DOUBLE_EQ(r.getRight(), 100.0);
    EXPECT_DOUBLE_EQ(r.getBottom(), 100.0);
    
    r.setCenter({50.0, 50.0});
    EXPECT_DOUBLE_EQ(r.getCenterX(), 50.0);
    EXPECT_DOUBLE_EQ(r.getCenterY(), 50.0);
    
    r.setMidTop({50.0, 10.0});
    EXPECT_DOUBLE_EQ(r.getTop(), 10.0);
    
    r.setMidBottom({50.0, 90.0});
    EXPECT_DOUBLE_EQ(r.getBottom(), 90.0);
    
    r.setMidLeft({10.0, 50.0});
    EXPECT_DOUBLE_EQ(r.getLeft(), 10.0);
    
    r.setMidRight({90.0, 50.0});
    EXPECT_DOUBLE_EQ(r.getRight(), 90.0);
}

TEST(RectWhiteBoxTest, CliplineEdgeCases) {
    sdlgame::rect::Rect r(10.0, 10.0, 80.0, 80.0);
    
    // Horizontal line inside
    auto res_horiz = r.clipline({20.0, 20.0}, {60.0, 20.0});
    ASSERT_TRUE(res_horiz.has_value());
    EXPECT_DOUBLE_EQ(res_horiz->first.x, 20.0);
    EXPECT_DOUBLE_EQ(res_horiz->first.y, 20.0);
    EXPECT_DOUBLE_EQ(res_horiz->second.x, 60.0);
    EXPECT_DOUBLE_EQ(res_horiz->second.y, 20.0);
    
    // Line going right to left
    auto res_rev = r.clipline({70.0, 70.0}, {20.0, 20.0});
    ASSERT_TRUE(res_rev.has_value());
    EXPECT_DOUBLE_EQ(res_rev->first.x, 70.0);
    EXPECT_DOUBLE_EQ(res_rev->first.y, 70.0);
    EXPECT_DOUBLE_EQ(res_rev->second.x, 20.0);
    EXPECT_DOUBLE_EQ(res_rev->second.y, 20.0);
}

TEST(RectWhiteBoxTest, SettersVector2) {
    sdlgame::rect::Rect r(0.0, 0.0, 10.0, 10.0);
    
    r.setTopLeft({5.0, 5.0});
    EXPECT_DOUBLE_EQ(r.getLeft(), 5.0);
    EXPECT_DOUBLE_EQ(r.getTop(), 5.0);
    
    r.setTopRight({20.0, 5.0});
    EXPECT_DOUBLE_EQ(r.getRight(), 20.0);
    EXPECT_DOUBLE_EQ(r.getTop(), 5.0);
    
    r.setBottomLeft({5.0, 20.0});
    EXPECT_DOUBLE_EQ(r.getLeft(), 5.0);
    EXPECT_DOUBLE_EQ(r.getBottom(), 20.0);
}

TEST(RectWhiteBoxTest, SettersDouble) {
    sdlgame::rect::Rect r(0.0, 0.0, 10.0, 10.0);
    
    r.setTopLeft(5.0, 5.0);
    EXPECT_DOUBLE_EQ(r.getLeft(), 5.0);
    EXPECT_DOUBLE_EQ(r.getTop(), 5.0);
    
    r.setTopRight(20.0, 5.0);
    EXPECT_DOUBLE_EQ(r.getRight(), 20.0);
    EXPECT_DOUBLE_EQ(r.getTop(), 5.0);
    
    r.setBottomLeft(5.0, 20.0);
    EXPECT_DOUBLE_EQ(r.getLeft(), 5.0);
    EXPECT_DOUBLE_EQ(r.getBottom(), 20.0);
    
    r.setBottomRight(30.0, 30.0);
    EXPECT_DOUBLE_EQ(r.getRight(), 30.0);
    EXPECT_DOUBLE_EQ(r.getBottom(), 30.0);
}

TEST(RectWhiteBoxTest, WidthHeightSizes) {
    sdlgame::rect::Rect r(0.0, 0.0, 10.0, 10.0);
    r.setWidth(20.0);
    EXPECT_DOUBLE_EQ(r.getWidth(), 20.0);
    EXPECT_DOUBLE_EQ(r.getLeft(), -5.0); // inflate_ip keeps center
    
    r.setHeight(30.0);
    EXPECT_DOUBLE_EQ(r.getHeight(), 30.0);
    EXPECT_DOUBLE_EQ(r.getTop(), -10.0); // inflate_ip keeps center
    
    r.setSize(40.0, 50.0);
    EXPECT_DOUBLE_EQ(r.getWidth(), 40.0);
    EXPECT_DOUBLE_EQ(r.getHeight(), 50.0);
    
    r.setSize({10.0, 10.0});
    EXPECT_DOUBLE_EQ(r.getWidth(), 10.0);
    EXPECT_DOUBLE_EQ(r.getHeight(), 10.0);
}

TEST(RectWhiteBoxTest, EdgeCaseCollisions) {
    sdlgame::rect::Rect r(10.0, 10.0, 40.0, 40.0);
    sdlgame::rect::Rect no_overlap(100.0, 100.0, 10.0, 10.0);
    
    r.overlap_ip(no_overlap);
    EXPECT_DOUBLE_EQ(r.getWidth(), 0.0);
    EXPECT_DOUBLE_EQ(r.getHeight(), 0.0);
}

TEST(RectWhiteBoxTest, SurfaceInteroperability) {
    // Test that the Rect interoperates with SDL objects and surfaces correctly
    sdlgame::rect::Rect r(10.0, 10.0, 50.0, 50.0);
    SDL_Rect sdl_rect = r.to_SDL_Rect();
    EXPECT_EQ(sdl_rect.x, 10);
    EXPECT_EQ(sdl_rect.y, 10);
    EXPECT_EQ(sdl_rect.w, 50);
    EXPECT_EQ(sdl_rect.h, 50);
    
    SDL_FRect sdl_frect = r.to_SDL_FRect();
    EXPECT_FLOAT_EQ(sdl_frect.x, 10.0f);
    EXPECT_FLOAT_EQ(sdl_frect.y, 10.0f);
    EXPECT_FLOAT_EQ(sdl_frect.w, 50.0f);
    EXPECT_FLOAT_EQ(sdl_frect.h, 50.0f);
}
