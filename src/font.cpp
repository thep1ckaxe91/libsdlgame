#include "font.hpp"
#include "color.hpp"
#include <iostream>
#include <exception>
#include "surface.hpp"
#include <SDL2/SDL_ttf.h>
#include <string>

namespace sdlgame::font {
void init() {
  if (TTF_Init()) [[unlikely]] {
    std::cerr << "Failed to init font\n" << TTF_GetError() << '\n';
    std::terminate();
  }
  std::cout << "Font successfully initialized\n";
  return;
}
Font::Font(const fs::path& path, int size) {
  m_height = size;
  auto new_font = TTF_OpenFont(path.string().c_str(), size);
  if (!new_font) {
    std::cerr << "Cant load font\n" << TTF_GetError() << '\n';
    std::terminate();
  }

  m_font.reset(new_font, memory::SDLDeleter{});
}

/**
 * @return a surface that only contain the text
 * @param antialias = 0 no antialiasing fastest
 *                  = 1 low antialiaing faster
 *                  = 2 high antialiasing slowest
 * the higher, the slower the render will be
 * @param wrap_length in pixel, once the text get over the wrap_length
 * it automatically endline, if it is default = 0,
 * then will only endline when use endline character
 *
 */
[[nodiscard]] sdlgame::surface::Surface Font::render(const std::string &text,
                                       AntiAlias antialias,
                                       sdlgame::color::Color color,
                                       uint32_t wrap_length,
                                       sdlgame::color::Color background) {
  sdlgame::memory::SDLUniquePtr<SDL_Surface> surface;
  switch (antialias) {
  case AntiAlias::SOLID:
    surface.reset(TTF_RenderUTF8_Solid_Wrapped(
        m_font.get(), text.c_str(), color.to_SDL_Color(), wrap_length));
    break;
  case AntiAlias::SHADED:
    surface.reset(TTF_RenderUTF8_Shaded_Wrapped(
        m_font.get(), text.c_str(), color.to_SDL_Color(), SDL_Color{0, 0, 0, 0},
        wrap_length));
    break;
  case AntiAlias::BLENDED:
    surface.reset(TTF_RenderUTF8_Blended_Wrapped(
        m_font.get(), text.c_str(), color.to_SDL_Color(), wrap_length));
    break;
  }
  if (!surface) [[unlikely]] {
    std::cerr << "Error render font\n" << TTF_GetError() << '\n';
    std::terminate();
  }
  surface::Surface res{surface.get()};
  
  return res;
}
int Font::get_height() const { return m_height; }

} // namespace sdlgame::font