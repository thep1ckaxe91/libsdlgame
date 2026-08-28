#include "display.hpp"
#include "engine.hpp"
#include "image.hpp"
#include "rect.hpp"
#include "sprite.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace sdlgame::sprite;
using namespace sdlgame::surface;
using namespace sdlgame::rect;

class TestSprite : public Sprite {
public:
  TestSprite() : Sprite() {}
  explicit TestSprite(const std::shared_ptr<const Surface> &image)
      : Sprite(image) {}

  void update() override {
    // Simple update logic for testing
    get_rect().setLeft(get_rect().getLeft() + 1);
  }
};

class TestSpriteRad : public Sprite {
public:
  double radius;

  TestSpriteRad() : Sprite() {}
  explicit TestSpriteRad(const std::shared_ptr<const Surface> &image)
      : Sprite(image) {}

  void update() override {
    // Simple update logic for testing
    get_rect().setLeft(get_rect().getLeft() + 1);
  }
};

class TestSpriteRadProp : public Sprite {

  double r;

public:
  TestSpriteRadProp() : Sprite() {}
  explicit TestSpriteRadProp(const std::shared_ptr<const Surface> &image)
      : Sprite(image) {}

  void update() override {
    // Simple update logic for testing
    get_rect().setLeft(get_rect().getLeft() + 1);
  }

  double radius() const { return r; }
  void set_rad(double r) { this->r = r; }
};

TEST(SpriteTest, DefaultConstructor) {
  auto sprite = std::make_shared<TestSprite>();
  EXPECT_FALSE(sprite->alive());
  EXPECT_EQ(sprite->groups().size(), 0);
}

TEST(SpriteTest, ConstructorWithImage) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);

  auto surf = sdlgame::image::load("assets/dummy.png");
  auto sprite = std::make_shared<TestSprite>(surf);
  EXPECT_FALSE(sprite->alive());
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

  static_assert( HasRadiusMember<TestSpriteRad>);
  static_assert( HasRadiusProperties<TestSpriteRadProp>);

  auto sprite3 = std::make_shared<TestSprite>();
  auto sprite4 = std::make_shared<TestSprite>();
  sprite3->get_rect() = Rect(0, 0, 10, 10); // center (5, 5)
  sprite4->get_rect() = Rect(8, 0, 10, 10); // center (13, 5)
  EXPECT_TRUE(collide_circle(*sprite3, *sprite4));

  auto sprite1 = std::make_shared<TestSpriteRad>();
  auto sprite2 = std::make_shared<TestSpriteRad>();
  sprite1->get_rect() = Rect(0, 0, 10, 10); // center (5, 5)
  sprite2->get_rect() = Rect(8, 0, 10, 10); // center (13, 5)
  // Test with specific radii
  sprite1->radius = 5;
  sprite2->radius = 5;
  EXPECT_TRUE(collide_circle(*sprite1, *sprite2)); // 5 + 5 = 10 >= 8
  sprite1->radius = 3;
  sprite2->radius = 3; // FIXME: this test is failing for some reason
  EXPECT_FALSE(collide_circle(*sprite1, *sprite2)); // 3 + 3 = 6 < 8

  auto sprite5 = std::make_shared<TestSpriteRadProp>();
  auto sprite6 = std::make_shared<TestSpriteRadProp>();
  sprite5->get_rect() = Rect(0, 0, 10, 10); // center (5, 5)
  sprite6->get_rect() = Rect(8, 0, 10, 10); // center (13, 5)
  // Test with specific radii
  sprite5->set_rad(5);
  sprite6->set_rad(5);
  EXPECT_TRUE(collide_circle(*sprite5, *sprite6)); // 5 + 5 = 10 >= 8
  sprite5->set_rad(3);
  sprite6->set_rad(3);
  EXPECT_FALSE(collide_circle(*sprite5, *sprite6)); // 3 + 3 = 6 < 8
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
