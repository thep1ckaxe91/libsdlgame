#include "display.hpp"
#include "engine.hpp"
#include "image.hpp"
#include "rect.hpp"
#include "sprite.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

using namespace sdlgame::sprite;
using namespace sdlgame::surface;
using namespace sdlgame::rect;

class TestSprite : public Sprite {
public:
  TestSprite() : Sprite() {}
  explicit TestSprite(const std::shared_ptr<Surface<SDL_TEXTUREACCESS_STATIC>> &image)
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
  explicit TestSpriteRad(const std::shared_ptr<Surface<SDL_TEXTUREACCESS_STATIC>> &image)
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
  explicit TestSpriteRadProp(const std::shared_ptr<Surface<SDL_TEXTUREACCESS_STATIC>> &image)
      : Sprite(image) {}

  void update() override {
    // Simple update logic for testing
    get_rect().setLeft(get_rect().getLeft() + 1);
  }

  double radius() const { return r; }
  void set_rad(double r) { this->r = r; }
};

class TestSpriteNonArithmetic : public Sprite {
public:
  std::string radius = "10";

  TestSpriteNonArithmetic() : Sprite() {}
  explicit TestSpriteNonArithmetic(const std::shared_ptr<Surface<SDL_TEXTUREACCESS_STATIC>> &image)
      : Sprite(image) {}

  void update() override {
    get_rect().setLeft(get_rect().getLeft() + 1);
  }
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
  auto sprite = std::make_shared<TestSprite>(std::make_shared<Surface<SDL_TEXTUREACCESS_STATIC>>(std::move(surf)));
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
  static_assert(!HasRadiusMember<TestSprite>);
  static_assert(!HasRadiusProperties<TestSprite>);
  static_assert(!HasRadiusMember<TestSpriteRadProp>);
  static_assert(!HasRadiusProperties<TestSpriteRad>);
  static_assert(!HasRadiusMember<TestSpriteNonArithmetic>);
  static_assert(!HasRadiusProperties<TestSpriteNonArithmetic>);

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
  sprite2->radius = 3;
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

// ================= WHITE-BOX TESTS =================

TEST(SpriteTest, GroupAddNullptr) {
  auto group = std::make_shared<Group>();
  group->add(nullptr);
  EXPECT_EQ(group->sprites().size(), 0);
}

TEST(SpriteTest, GroupAddDuplicate) {
  auto group = std::make_shared<Group>();
  auto sprite = std::make_shared<TestSprite>();
  group->add(sprite);
  group->add(sprite);
  EXPECT_EQ(group->sprites().size(), 1);
  EXPECT_EQ(sprite->groups().size(), 1);
}

TEST(SpriteTest, GroupRemoveNonExistent) {
  auto group = std::make_shared<Group>();
  auto sprite = std::make_shared<TestSprite>();
  group->remove(sprite);
  EXPECT_EQ(group->sprites().size(), 0);
}

TEST(SpriteTest, GroupRemoveCleansExpiredWeakPtrs) {
  auto group1 = std::make_shared<Group>();
  auto sprite = std::make_shared<TestSprite>();
  
  {
    auto group2 = std::make_shared<Group>();
    sprite->add(group1);
    sprite->add(group2);
  }
  
  group1->remove(sprite);
  EXPECT_EQ(sprite->groups().size(), 0);
}

TEST(SpriteTest, GroupUpdate) {
  auto group = std::make_shared<Group>();
  auto sprite1 = std::make_shared<TestSprite>();
  auto sprite2 = std::make_shared<TestSprite>();
  
  sprite1->get_rect() = Rect(0, 0, 10, 10);
  sprite2->get_rect() = Rect(0, 0, 10, 10);
  
  group->add(sprite1);
  group->add(sprite2);
  
  group->update();
  
  EXPECT_EQ(sprite1->get_rect().getLeft(), 1);
  EXPECT_EQ(sprite2->get_rect().getLeft(), 1);
}

TEST(SpriteTest, GroupDraw) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);

  auto surf = sdlgame::image::load("assets/dummy.png");
  auto dest = std::make_shared<Surface<SDL_TEXTUREACCESS_TARGET>>(600, 400);
  auto sprite = std::make_shared<TestSprite>(std::make_shared<Surface<SDL_TEXTUREACCESS_STATIC>>(std::move(surf)));
  
  auto group = std::make_shared<Group>();
  group->add(sprite);
  
  auto sprite_no_image = std::make_shared<TestSprite>();
  group->add(sprite_no_image);
  
  group->draw(*dest);
  EXPECT_TRUE(true);
}

TEST(SpriteTest, GroupIterators) {
  auto group = std::make_shared<Group>();
  auto sprite1 = std::make_shared<TestSprite>();
  auto sprite2 = std::make_shared<TestSprite>();
  group->add(sprite1);
  group->add(sprite2);
  
  int count = 0;
  for (auto it = group->begin(); it != group->end(); ++it) {
    count++;
  }
  EXPECT_EQ(count, 2);
}

TEST(SpriteTest, SpriteAddRemoveGroup) {
  auto group = std::make_shared<Group>();
  auto sprite = std::make_shared<TestSprite>();
  
  sprite->add(group);
  EXPECT_TRUE(group->has(sprite));
  EXPECT_EQ(sprite->groups().size(), 1);
  
  sprite->remove(group);
  EXPECT_FALSE(group->has(sprite));
  EXPECT_EQ(sprite->groups().size(), 0);
}

TEST(SpriteTest, SpriteGroupsCache) {
  auto sprite = std::make_shared<TestSprite>();
  auto group1 = std::make_shared<Group>();
  auto group2 = std::make_shared<Group>();
  
  sprite->add(group1);
  sprite->add(group2);
  
  auto groups = sprite->groups();
  EXPECT_EQ(groups.size(), 2);
  
  auto groups2 = sprite->groups();
  EXPECT_EQ(groups2.size(), 2);
  
  sprite->remove(group1);
  auto groups3 = sprite->groups();
  EXPECT_EQ(groups3.size(), 1);
  EXPECT_EQ(groups3[0], group2);
}

TEST(SpriteTest, SpriteGroupsExpiredWeakPtr) {
  auto sprite = std::make_shared<TestSprite>();
  {
    auto group = std::make_shared<Group>();
    group->add(sprite);
  }
  
  auto groups = sprite->groups();
  EXPECT_EQ(groups.size(), 0);
}

TEST(SpriteTest, SpriteConstGetRect) {
  auto sprite = std::make_shared<TestSprite>();
  sprite->get_rect() = Rect(1, 2, 3, 4);
  
  const TestSprite& const_sprite = *sprite;
  EXPECT_EQ(const_sprite.get_rect().getLeft(), 1);
}

TEST(SpriteTest, SpriteGetImage) {
  sdlgame::init();
  sdlgame::display::set_mode(600, 400);

  auto surf = sdlgame::image::load("assets/dummy.png");
  auto sprite = std::make_shared<TestSprite>(std::make_shared<Surface<SDL_TEXTUREACCESS_STATIC>>(std::move(surf)));
  
  const auto& img = sprite->get_image();
  EXPECT_EQ(img.get_rect().getWidth(), surf.get_rect().getWidth());
}

TEST(SpriteTest, GroupSingleAddNullptr) {
  auto group = std::make_shared<GroupSingle>();
  group->add(nullptr);
  EXPECT_EQ(group->sprites().size(), 0);
}

TEST(SpriteTest, GroupSingleConstructNullptr) {
  auto group = std::make_shared<GroupSingle>(nullptr);
  EXPECT_EQ(group->sprites().size(), 0);
}

TEST(SpriteTest, GroupSingleAddSame) {
  auto group = std::make_shared<GroupSingle>();
  auto sprite = std::make_shared<TestSprite>();
  group->add(sprite);
  group->add(sprite);
  EXPECT_EQ(group->sprites().size(), 1);
  EXPECT_EQ(sprite->groups().size(), 1);
}

TEST(SpriteTest, GroupSingleRemoveEmpty) {
  auto group = std::make_shared<GroupSingle>();
  group->remove(); 
  EXPECT_EQ(group->sprites().size(), 0);
}

TEST(SpriteTest, SpriteCollideEmptyGroup) {
  auto group = std::make_shared<Group>();
  auto sprite = std::make_shared<TestSprite>();
  auto collided = spritecollide(sprite, group, false);
  EXPECT_TRUE(collided.empty());
}

TEST(SpriteTest, CollideCircleMixedTypes) {
  auto sprite_def = std::make_shared<TestSprite>();
  sprite_def->get_rect() = Rect(0, 0, 10, 10); // default radius is sqrt(5^2 + 5^2) = 7.07

  auto sprite_rad = std::make_shared<TestSpriteRad>();
  sprite_rad->get_rect() = Rect(12, 0, 10, 10); // center (17, 5). dx = 12, dy = 0.
  
  // 7.07 + 5.0 = 12.07 > 12 -> True
  sprite_rad->radius = 5.0;
  EXPECT_TRUE(collide_circle(*sprite_def, *sprite_rad));
  
  // 7.07 + 4.9 = 11.97 < 12 -> False
  sprite_rad->radius = 4.9;
  EXPECT_FALSE(collide_circle(*sprite_def, *sprite_rad));

  auto sprite_prop = std::make_shared<TestSpriteRadProp>();
  sprite_prop->get_rect() = Rect(0, 0, 10, 10); // center (5, 5). dx = 12, dy = 0.
  
  // 4.9 + 7.2 = 12.1 > 12 -> True
  sprite_prop->set_rad(7.2);
  EXPECT_TRUE(collide_circle(*sprite_prop, *sprite_rad));
  
  // 4.9 + 7.0 = 11.9 < 12 -> False
  sprite_prop->set_rad(7.0);
  EXPECT_FALSE(collide_circle(*sprite_prop, *sprite_rad));
}
