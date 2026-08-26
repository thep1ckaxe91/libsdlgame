#include "mixer.hpp"
#include <SDL2/SDL_mixer.h>
#include <exception>
#include <iostream>
#include <utility>

namespace sdlgame::mixer {
void set_num_channels(int count) { Mix_AllocateChannels(count); }
void init(int freq, uint16_t size, int channels, int buffer) {
  size = (size == 16 ? AUDIO_S16SYS : AUDIO_F32SYS);
  if ((Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3) != MIX_INIT_MP3) {
    std::cerr << "Failed to init mp3 type\n" << Mix_GetError() << '\n';
  }
  if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG) {
    std::cerr << "Failed to init ogg type\n" << Mix_GetError() << '\n';
  }
  if ((Mix_Init(MIX_INIT_WAVPACK) & MIX_INIT_WAVPACK) != MIX_INIT_WAVPACK) {
    std::cerr << "Failed to init WAV pack\n" << Mix_GetError() << '\n';
  }
  if (Mix_OpenAudio(freq, size, channels, buffer)) {
    std::cerr << "Failed to init mixer\n" << Mix_GetError() << '\n';
    std::terminate();
  } else {
    std::cout << "Mixer successfully initialized\n";
  }
}
int get_num_channels() { return Mix_AllocateChannels(-1); }

int convert_volume_value(float value) {
  return int(std::min(std::max(value, 0.f), 1.f) * MIX_MAX_VOLUME);
}

Channel::Channel(int _id) : id(_id), volume(1.0f) {}

void Channel::play(const Sound& sound, int loops, int maxtime_ms, int fade_ms) {
  if (Mix_FadeInChannelTimed(id, sound.chunk.get(), loops, fade_ms,
                             maxtime_ms) == -1) [[unlikely]] {
    std::cerr << "No channel available\n" << Mix_GetError() << '\n';
    std::terminate();
  }
}
void Channel::set_volume(float value) {
  Mix_Volume(id, convert_volume_value(value));
}
int Channel::get_volume() const { return Mix_Volume(id, -1); }

Sound::Sound() : channel(-1), volume(1.0f) {}
Sound::Sound(const fs::path &path) : channel(-1), volume(1.0f) {
  auto new_chunk = Mix_LoadWAV(path.string().c_str());
  if (!new_chunk) [[unlikely]] {
    std::cerr << "Cant load track\n" << Mix_GetError() << '\n';
    std::terminate();
  }
  chunk.reset(new_chunk, memory::SDLDeleter{});
}
Sound::Sound(Sound &&oth) noexcept
    : channel(oth.channel), volume(oth.volume), chunk(std::move(oth.chunk)) {}

Sound &Sound::operator=(const Sound &oth) {
  chunk = oth.chunk;
  volume = oth.volume;
  channel = oth.channel;
  return *this;
}

Sound &Sound::operator=(Sound &&oth) noexcept {
  chunk = std::move(oth.chunk);
  volume = oth.volume;
  channel = oth.channel;
  return *this;
}

Channel Sound::play(int loops, int maxtime_ms, int fade_ms) {
  Mix_VolumeChunk(this->chunk.get(), convert_volume_value(volume));
  channel = Mix_FadeInChannelTimed(-1, chunk.get(), loops, fade_ms, maxtime_ms);
  if (channel == -1) [[unlikely]] {
    std::cout << "No channel available\n" << Mix_GetError() << '\n';
  }

  Channel c{channel};
  return c;
}
void Sound::load(const fs::path &path) {
  auto new_chunk = Mix_LoadWAV(path.string().c_str());
  if (!new_chunk) [[unlikely]] {
    std::cerr << "Cant load track\n" << Mix_GetError() << '\n';
    std::terminate();
  }
  chunk.reset(new_chunk, memory::SDLDeleter{});
}
void Sound::fadeout(int ms) { Mix_FadeOutChannel(channel, ms); }

void Sound::set_volume(float value) { volume = value; }
float Sound::get_volume() const { return volume; }
} // namespace sdlgame::mixer

