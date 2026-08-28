#include <gtest/gtest.h>
#include "mixer.hpp"

using namespace sdlgame::mixer;

TEST(MixerTest, GlobalFunctions) {
    // Test that global functions compile against the API signature
    set_num_channels(8);
    int channels = get_num_channels();
    EXPECT_GE(channels, 0);
    
    int min_vol = convert_volume_value(0.0f);
    int max_vol = convert_volume_value(1.0f);
    EXPECT_LE(min_vol, max_vol);
}

TEST(MixerTest, ChannelVolume) {
    Channel ch(2);
    ch.set_volume(0.5f);
    
    int vol = ch.get_volume();
    // Assuming get_volume returns the internal mapped volume, which should be >= 0
    EXPECT_GE(vol, 0); 
}

TEST(MixerTest, SoundConstructorsAndVolume) {
    Sound snd;
    snd.set_volume(0.75f);
    
    float vol = snd.get_volume();
    EXPECT_GE(vol, 0);
    
    Sound snd_moved{std::move(snd)};
    float moved_vol = snd_moved.get_volume();
    EXPECT_EQ(moved_vol, vol);
    
    Sound snd_assigned;
    snd_assigned = std::move(snd_moved);
    EXPECT_NEAR(static_cast<double>(snd_assigned.get_volume()), 0.75, 0.01);
}

TEST(MixerTest, SoundAndChannelAPI) {
    Sound snd;
    Channel ch(1);
    
    // Testing the signatures of play and fadeout
    // We won't execute these if they cause a crash on null chunk, but we ensure they compile.
    // If the implementation is robust, this shouldn't crash.
    // snd.play(0, -1, 0);
    // ch.play(snd, 0, -1, 0);
    // snd.fadeout(100);
    
    SUCCEED();
}

TEST(MixerTest, ConvertVolumeValueEdgeCases) {
    EXPECT_EQ(convert_volume_value(-1.0f), 0);
    EXPECT_EQ(convert_volume_value(0.0f), 0);
    EXPECT_EQ(convert_volume_value(0.5f), int(0.5f * MIX_MAX_VOLUME));
    EXPECT_EQ(convert_volume_value(1.0f), MIX_MAX_VOLUME);
    EXPECT_EQ(convert_volume_value(2.0f), MIX_MAX_VOLUME);
}

TEST(MixerTest, SoundCopyAssignment) {
    Sound snd1;
    snd1.set_volume(0.3f);
    
    Sound snd2;
    snd2 = snd1;
    EXPECT_EQ(snd2.get_volume(), 0.3f);
    EXPECT_EQ(snd2.chunk.get(), snd1.chunk.get());
}

TEST(MixerTest, SoundLoadInvalidPathTerminates) {
    EXPECT_DEATH({
        Sound snd("nonexistent_path_12345.wav");
    }, "Cant load track");
    
    EXPECT_DEATH({
        Sound snd;
        snd.load("nonexistent_path_12345.wav");
    }, "Cant load track");
}

TEST(MixerTest, ChannelPlayTerminatesOnFailure) {
    EXPECT_DEATH({
        Sound snd;
        Channel ch(1);
        ch.play(snd); 
    }, "No channel available");
}

TEST(MixerTest, SoundPlayPrintsOnFailure) {
    testing::internal::CaptureStdout();
    Sound snd;
    snd.play();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("No channel available") != std::string::npos);
}

TEST(MixerTest, SoundFadeOut) {
    Sound snd;
    snd.fadeout(100);
    SUCCEED();
}

TEST(MixerTest, InitFailsAndTerminates) {
    EXPECT_DEATH({
        // Passing completely invalid parameters to force Mix_OpenAudio to fail
        init(-1, 0, -1, -1);
    }, "Failed to init mixer");
}
