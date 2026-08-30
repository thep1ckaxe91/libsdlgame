#include <gtest/gtest.h>
#include "display.hpp"
#include "image.hpp"
#include <filesystem>
#include "engine.hpp"

TEST(ImageTest, InitDoesNotThrow) {
    EXPECT_NO_THROW({
        sdlgame::image::init();
    });
}

TEST(ImageTest, LoadNonExistentPathReturnsNullOrThrows) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    using namespace sdlgame;

    std::filesystem::path fakePath = "fake/path.png";
    EXPECT_DEATH({
        auto surf = sdlgame::image::load(fakePath);
    }, ".*FATAL: SDL Error.*");
}

TEST(ImageTest, LoadWithValidPathStringCompiles) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    auto surf = sdlgame::image::load("assets/dummy.jpg");
    EXPECT_NE(surf.getTexture(), nullptr);
}

TEST(ImageTest, LoadWithNullRendererDies) {
    sdlgame::init();
    sdlgame::display::set_mode(600, 400);
    // Force renderer to be null to test error handling path
    sdlgame::display::quit();
    
    EXPECT_DEATH({
        auto surf = sdlgame::image::load("assets/dummy.jpg");
    }, ".*FATAL: SDL Error.*");
}

TEST(ImageTest, LoadWithEmptyPathDies) {
    sdlgame::init();
    sdlgame::display::set_mode(600, 400);
    
    EXPECT_DEATH({
        auto surf = sdlgame::image::load("");
    }, ".*FATAL: SDL Error.*");
}

TEST(ImageTest, DoubleInitDoesNotThrow) {
    EXPECT_NO_THROW({
        sdlgame::image::init();
        sdlgame::image::init();
    });
}

TEST(ImageTest, LoadMultipleTimesSameImage) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    auto surf1 = sdlgame::image::load("assets/dummy.jpg");
    EXPECT_NE(surf1.getTexture(), nullptr);
    
    auto surf2 = sdlgame::image::load("assets/dummy.jpg");
    EXPECT_NE(surf2.getTexture(), nullptr);
    EXPECT_NE(surf1.getTexture(), surf2.getTexture());
}

