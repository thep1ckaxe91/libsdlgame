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
    std::shared_ptr<const surface::Surface> surf;
    EXPECT_DEATH(surf = sdlgame::image::load(fakePath);, "");
}

TEST(ImageTest, LoadWithValidPathStringCompiles) {
    sdlgame::init();
    sdlgame::display::set_mode(600,400);
    auto surf = sdlgame::image::load("assets/dummy.jpg");
    
}
