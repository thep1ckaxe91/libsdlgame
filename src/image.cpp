#include "image.hpp"
#include "SDL_image.h"
#include "display.hpp"
#include "surface.hpp"
#include <filesystem>
#include <iostream>
#include <exception>

namespace fs = std::filesystem;

namespace sdlgame::image {
void init() {
  if ((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) != IMG_INIT_JPG) {
    std::cerr << "Failed to init JPG image flags\n" << IMG_GetError() << '\n';
    std::terminate();
  } else if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    std::cerr << "Failed to init PNG image flags\n" << IMG_GetError() << '\n';
    std::terminate();
  } else {
    std::cout << "Image successfully initialized\n";
    return;
  }
}
// sdlgame::surface::Surface img_transfer_surf;
[[nodiscard]] std::shared_ptr<const surface::Surface> load(const fs::path path) {
  auto tex = IMG_LoadTexture(sdlgame::display::get_renderer(), path.string().c_str());

  if (!tex) {
    std::cerr << "Cant load image\n" << IMG_GetError() << '\n';
    std::terminate();
  }
  return std::make_shared<const surface::Surface>(tex);
}
} // namespace sdlgame::image
