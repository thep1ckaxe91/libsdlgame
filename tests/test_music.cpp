#include <gtest/gtest.h>
#include "music.hpp"
#include "mixer.hpp"

TEST(MusicTest, SetGetVolume) {
    sdlgame::mixer::init();
    sdlgame::music::set_volume(0.75f);
    EXPECT_FLOAT_EQ(sdlgame::music::get_volume(), 0.75f);
    
    sdlgame::music::set_volume(0.25f);
    EXPECT_FLOAT_EQ(sdlgame::music::get_volume(), 0.25f);
}

TEST(MusicTest, ConvertVolumeValue) {
    sdlgame::mixer::init();
    int vol_0 = sdlgame::music::convert_volume_value(0.0f);
    int vol_50 = sdlgame::music::convert_volume_value(0.5f);
    int vol_100 = sdlgame::music::convert_volume_value(1.0f);
    
    EXPECT_GE(vol_50, vol_0);
    EXPECT_LE(vol_50, vol_100);
}

TEST(MusicTest, PlaybackState) {
    sdlgame::mixer::init();
    EXPECT_NO_THROW({
        sdlgame::music::stop();
        sdlgame::music::pause();
        sdlgame::music::resume();
    });
    
    EXPECT_FALSE(sdlgame::music::is_playing());
}

TEST(MusicTest, Duration) {
    sdlgame::mixer::init();
    double dur = 0.0;
    EXPECT_NO_THROW({
        dur = sdlgame::music::duration();
    });
    
    EXPECT_EQ(dur, -1);

    sdlgame::music::load(fs::path{"assets/dummy.mp3"});

    dur = sdlgame::music::duration();
    EXPECT_GE(dur, 0.1);
}
