#include "engine.hpp"
#include "font.hpp"
#include "image.hpp"
#include "key.hpp"
#include <SDL_image.h>
#include <iostream>

void sdlgame::init() {
  SDL_CHECK(SDL_Init(SDL_INIT_EVERYTHING));
  std::cout << "SDL successfully initialized\n";
  sdlgame::image::init();
  sdlgame::font::init();
  sdlgame::key::init();
}
void sdlgame::quit() {
  IMG_Quit();
  Mix_Quit();
  TTF_Quit();
  SDL_Quit();
}
fs::path sdlgame::get_base_path() {
  static fs::path p;
  if (p.empty()) {
    char *base_path = SDL_NEW(SDL_GetBasePath());
    p = base_path;
    SDL_free(base_path);
  }
  return p;
}