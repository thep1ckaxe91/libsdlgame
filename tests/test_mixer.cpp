#include <filesystem>
#include <gtest/gtest.h>
#include "mixer.hpp"
#include "engine.hpp"

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
    sdlgame::init();
    sdlgame::mixer::init();
    Sound snd{fs::path{"assets"} / "dummy.mp3"};
    Channel ch(1);
    
    // Testing the signatures of play and fadeout
    snd.play(0, -1, 0);
    ch.play(snd, 0, -1, 0);
    snd.fadeout(100);
    
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
    }, "FATAL: SDL Error");
    
    EXPECT_DEATH({
        Sound snd;
        snd.load("nonexistent_path_12345.wav");
    }, "FATAL: SDL Error");
}

TEST(MixerTest, ChannelPlayTerminatesOnFailure) {
    EXPECT_DEATH({
        Sound snd;
        Channel ch(1);
        ch.play(snd); 
    }, "FATAL: SDL Error");
}

TEST(MixerTest, SoundPlayTerminatesOnFailure) {
    EXPECT_DEATH({
        Sound snd;
        snd.play();
    }, "FATAL: SDL Error");
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
    }, "FATAL: SDL Error");
}

TEST(MixerTest, InitFailsAndTerminatesSize32) {
    EXPECT_DEATH({
        // Passing 32 for size to cover the AUDIO_F32SYS branch
        init(-1, 32, -1, -1);
    }, "FATAL: SDL Error");
}

TEST(MixerTest, SoundChunkSharedPtrSemantics) {
    Sound snd1;
    Mix_Chunk dummy;
    // Inject a dummy chunk with a no-op deleter to test shared_ptr sharing 
    // without triggering SDL_FreeChunk on invalid memory.
    snd1.chunk = sdlgame::memory::SDLSharedPtr<Mix_Chunk>(&dummy, [](Mix_Chunk*){});
    snd1.set_volume(0.6f);
    
    EXPECT_EQ(snd1.chunk.use_count(), 1);
    
    Sound snd2;
    snd2 = snd1; // copy assignment
    
    EXPECT_EQ(snd1.chunk.use_count(), 2);
    EXPECT_EQ(snd2.chunk.use_count(), 2);
    EXPECT_EQ(snd2.chunk.get(), &dummy);
    EXPECT_EQ(snd2.get_volume(), 0.6f); // check volume is copied
    
    Sound snd3(std::move(snd1)); // move construction
    EXPECT_EQ(snd3.chunk.use_count(), 2);
    EXPECT_EQ(snd1.chunk.use_count(), 0);
    EXPECT_EQ(snd1.chunk.get(), nullptr);
    EXPECT_EQ(snd3.get_volume(), 0.6f);
    
    Sound snd4;
    snd4 = std::move(snd2); // move assignment
    EXPECT_EQ(snd4.chunk.use_count(), 2);
    EXPECT_EQ(snd2.chunk.use_count(), 0);
    EXPECT_EQ(snd2.chunk.get(), nullptr);
    EXPECT_EQ(snd4.get_volume(), 0.6f);
}

TEST(MixerTest, SoundPlayWithParametersTerminatesOnFailure) {
    EXPECT_DEATH({
        Sound snd;
        Channel c = snd.play(2, 1000, 500);
    }, "FATAL: SDL Error");
}

TEST(MixerTest, ChannelPlayWithParametersTerminates) {
    EXPECT_DEATH({
        Sound snd;
        Channel ch(1);
        ch.play(snd, 2, 2000, 100); 
    }, "FATAL: SDL Error");
}

