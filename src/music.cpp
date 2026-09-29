#include "music.hpp"
#include "engine.hpp"
#include "SDL2/SDL_mixer.h"
#include "memory.hpp"
#include <algorithm>

namespace sdlgame::music {
static memory::SDLUniquePtr<Mix_Music> music;

void load(const fs::path& path) {
  music.reset(SDL_NEW(Mix_LoadMUS(path.string().c_str())));
}
void play(int loop, int fadein_ms) {
  if (!music)
    return;
  SDL_CHECK(Mix_FadeInMusic(music.get(), loop, fadein_ms));
}
void pause() { Mix_PauseMusic(); }
void resume() { Mix_ResumeMusic(); }
void stop() { Mix_HaltMusic(); }
bool is_playing() { return Mix_PlayingMusic() != 0; }
double duration() {
  return SDL_CHECK(Mix_MusicDuration(music.get())); 
}
int convert_volume_value(float value) {
  return static_cast<int>(std::clamp(value, 0.0f, 1.0f) * MIX_MAX_VOLUME);
}
void set_volume(float value) { Mix_VolumeMusic(convert_volume_value(value)); }
float get_volume() {
  return static_cast<float>(Mix_VolumeMusic(-1)) / MIX_MAX_VOLUME;
}
} // namespace sdlgame::music
