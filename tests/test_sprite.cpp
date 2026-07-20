#include <gtest/gtest.h>
#include "sprite.hpp"
#include "surface.hpp"
#include "rect.hpp"
#include <memory>
#include <vector>

using namespace sdlgame::sprite;
using namespace sdlgame::surface;
using namespace sdlgame::rect;

class TestSprite : public Sprite {
public:
    TestSprite() : Sprite() {}
    explicit TestSprite(const std::shared_ptr<const Surface>& image) : Sprite(image) {}
    
    void update() override {
        // Simple update logic for testing
        get_rect().setLeft(get_rect().getLeft() + 1);
    }
};

TEST(SpriteTest, DefaultConstructor) {
    auto sprite = std::make_shared<TestSprite>();
    EXPECT_TRUE(sprite->alive());
    EXPECT_EQ(sprite->groups().size(), 0);
}

TEST(SpriteTest, ConstructorWithImage) {
    auto surf = std::make_shared<Surface>(100, 100);
    auto sprite = std::make_shared<TestSprite>(surf);
    EXPECT_TRUE(sprite->alive());
}

TEST(SpriteTest, GroupAddRemove) {
    auto group = std::make_shared<Group>();
    auto sprite = std::make_shared<TestSprite>();
    
    group->add(sprite);
    EXPECT_TRUE(group->has(sprite));
    EXPECT_EQ(sprite->groups().size(), 1);
    
    group->remove(sprite);
    EXPECT_FALSE(group->has(sprite));
    EXPECT_EQ(sprite->groups().size(), 0);
}

TEST(SpriteTest, SpriteKill) {
    auto group1 = std::make_shared<Group>();
    auto group2 = std::make_shared<Group>();
    auto sprite = std::make_shared<TestSprite>();
    
    group1->add(sprite);
    group2->add(sprite);
    EXPECT_EQ(sprite->groups().size(), 2);
    EXPECT_TRUE(sprite->alive());
    
    sprite->kill();
    EXPECT_FALSE(sprite->alive());
    EXPECT_EQ(sprite->groups().size(), 0);
    EXPECT_FALSE(group1->has(sprite));
    EXPECT_FALSE(group2->has(sprite));
}

TEST(SpriteTest, GroupSingle) {
    auto group_single = std::make_shared<GroupSingle>();
    auto sprite1 = std::make_shared<TestSprite>();
    auto sprite2 = std::make_shared<TestSprite>();
    
    group_single->add(sprite1);
    EXPECT_TRUE(group_single->has(sprite1));
    EXPECT_EQ(group_single->sprites().size(), 1);
    
    group_single->add(sprite2);
    EXPECT_FALSE(group_single->has(sprite1)); // Replaced
    EXPECT_TRUE(group_single->has(sprite2));
    EXPECT_EQ(group_single->sprites().size(), 1);
    
    group_single->remove(); // remove current sprite
    EXPECT_FALSE(group_single->has(sprite2));
    EXPECT_EQ(group_single->sprites().size(), 0);
}

TEST(SpriteTest, CollideRect) {
    auto sprite1 = std::make_shared<TestSprite>();
    sprite1->get_rect() = Rect(0, 0, 10, 10);
    
    auto sprite2 = std::make_shared<TestSprite>();
    sprite2->get_rect() = Rect(5, 5, 10, 10);
    
    EXPECT_TRUE(collide_rect(*sprite1, *sprite2));
    
    auto sprite3 = std::make_shared<TestSprite>();
    sprite3->get_rect() = Rect(20, 20, 10, 10);
    EXPECT_FALSE(collide_rect(*sprite1, *sprite3));
}

TEST(SpriteTest, CollideCircle) {
    auto sprite1 = std::make_shared<TestSprite>();
    sprite1->get_rect() = Rect(0, 0, 10, 10); // center (5, 5)
    
    auto sprite2 = std::make_shared<TestSprite>();
    sprite2->get_rect() = Rect(8, 0, 10, 10); // center (13, 5)
    
    EXPECT_TRUE(collide_circle(*sprite1, *sprite2));
    
    // Test with specific radii
    EXPECT_TRUE(collide_circle(*sprite1, *sprite2, 5.0, 5.0)); // 5 + 5 = 10 >= 8
    EXPECT_FALSE(collide_circle(*sprite1, *sprite2, 3.0, 3.0)); // 3 + 3 = 6 < 8
}

TEST(SpriteTest, SpriteCollideFunction) {
    auto group = std::make_shared<Group>();
    auto sprite1 = std::make_shared<TestSprite>();
    sprite1->get_rect() = Rect(0, 0, 10, 10);
    
    auto sprite2 = std::make_shared<TestSprite>();
    sprite2->get_rect() = Rect(5, 5, 10, 10);
    
    auto sprite3 = std::make_shared<TestSprite>();
    sprite3->get_rect() = Rect(20, 20, 10, 10);
    
    group->add(sprite1);
    group->add(sprite2);
    group->add(sprite3);
    
    auto target = std::make_shared<TestSprite>();
    target->get_rect() = Rect(0, 0, 10, 10);
    
    // dokill = false
    auto collided = spritecollide(target, group, false);
    EXPECT_EQ(collided.size(), 2); // target collides with sprite1 and sprite2
    EXPECT_TRUE(group->has(sprite1));
    EXPECT_TRUE(group->has(sprite2));
    
    // dokill = true
    collided = spritecollide(target, group, true);
    EXPECT_EQ(collided.size(), 2);
    EXPECT_FALSE(group->has(sprite1));
    EXPECT_FALSE(group->has(sprite2));
    EXPECT_TRUE(group->has(sprite3)); // still in group because it didn't collide
}
