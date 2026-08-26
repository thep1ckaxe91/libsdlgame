#pragma once
#ifndef SDLGAME_MUSIC_
#define SDLGAME_MUSIC_
#include "SDL2/SDL_mixer.h"
#include "memory.hpp"
#include <filesystem>

namespace fs = std::filesystem;

namespace sdlgame::music {
void load(const fs::path &path);
void play(int loop = 0, int fadein_ms = 0);
void pause();
void resume();
void stop();
bool is_playing();
double duration();
void set_volume(float value);
float get_volume();
int convert_volume_value(float value);
} // namespace sdlgame::music

#endif