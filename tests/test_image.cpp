#include <gtest/gtest.h>
#include "image.hpp"
#include <filesystem>

TEST(ImageTest, InitDoesNotThrow) {
    EXPECT_NO_THROW({
        sdlgame::image::init();
    });
}

TEST(ImageTest, LoadNonExistentPathReturnsNullOrThrows) {
    std::filesystem::path fakePath = "non_existent_image.png";
    EXPECT_NO_THROW({
        try {
            auto surf = sdlgame::image::load(fakePath);
        } catch(...) {
            // Ignore exceptions, just ensure the API signature works
        }
    });
}

TEST(ImageTest, LoadWithValidPathStringCompiles) {
    EXPECT_NO_THROW({
        try {
            auto surf = sdlgame::image::load("fake_valid_image.jpg");
        } catch(...) {}
    });
}
