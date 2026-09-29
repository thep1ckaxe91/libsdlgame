#include "image.hpp"
#include "SDL_image.h"
#include "display.hpp"
#include "surface.hpp"
#include <SDL_render.h>
#include "engine.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace sdlgame::image {
void init() {
  SDL_CHECK(((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) == IMG_INIT_JPG) ? 0 : -1);
  SDL_CHECK(((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == IMG_INIT_PNG) ? 0 : -1);
  std::cout << "Image successfully initialized\n";
}
// sdlgame::surface::Surface img_transfer_surf;
[[nodiscard]] surface::Surface<SDL_TEXTUREACCESS_STATIC>
load(const fs::path &path) {
  auto tex = SDL_NEW(
      IMG_LoadTexture(sdlgame::display::get_renderer(), path.string().c_str()));
  return surface::Surface<SDL_TEXTUREACCESS_STATIC>{tex};
}
} // namespace sdlgame::image
