#include "font.hpp"
#include "color.hpp"
#include "display.hpp"
#include "engine.hpp"
#include "memory.hpp"
#include "surface.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL_hints.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <iostream>
#include <string>

namespace sdlgame::font {
void init() {
  SDL_CHECK(TTF_Init());
  std::cout << "Font successfully initialized\n";
  return;
}
Font::Font(const fs::path &path, int size) {
  m_height = size;
  auto new_font = SDL_NEW(TTF_OpenFont(path.string().c_str(), size));

  m_font.reset(new_font, memory::SDLDeleter{});
}

/**
 * @return a surface that only contain the text
 * @param antialias = 0 no antialiasing fastest (background color had no effect
 * since colored with 0/1) = 1 low antialiaing faster = 2 high antialiasing
 * slowest the higher, the slower the render will be
 * @param wrap_length in pixel, once the text get over the wrap_length
 * it automatically endline, if it is default = 0,
 * then will only endline when use endline character
 *
 */
[[nodiscard]] surface::Surface<SDL_TEXTUREACCESS_STATIC>
Font::render(const std::string &text, AntiAlias antialias,
             sdlgame::color::Color color, uint32_t wrap_length,
             sdlgame::color::Color background) {

  sdlgame::memory::SDLUniquePtr<SDL_Surface> surface;

  switch (antialias) {
  case AntiAlias::SOLID:
    surface.reset(SDL_NEW(TTF_RenderUTF8_Solid_Wrapped(
        m_font.get(), text.c_str(), color.to_SDL_Color(), wrap_length)));
    break;
  case AntiAlias::SHADED:
    surface.reset(SDL_NEW(TTF_RenderUTF8_Shaded_Wrapped(
        m_font.get(), text.c_str(), color.to_SDL_Color(),
        background.to_SDL_Color(), wrap_length)));
    break;
  case AntiAlias::BLENDED:
    surface.reset(SDL_NEW(TTF_RenderUTF8_LCD_Wrapped(
        m_font.get(), text.c_str(), color.to_SDL_Color(),
        background.to_SDL_Color(), wrap_length)));
    break;
  }
  
  auto tex = SDL_NEW(SDL_CreateTextureFromSurface(
      sdlgame::display::get_renderer(), surface.get()));

  return surface::Surface<SDL_TEXTUREACCESS_STATIC>{tex};
}
int Font::get_height() const { return m_height; }

} // namespace sdlgame::font