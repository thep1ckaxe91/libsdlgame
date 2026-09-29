#pragma once
#ifndef SDLGAME_COLOR_
#define SDLGAME_COLOR_
#include "SDL2/SDL_pixels.h"
#include <concepts>
#include <cstdint>
#include <string>

namespace sdlgame::color {

/**
 *  class for color, all values range from [0,255]
 */
class Color {
public:
  uint8_t r = 0, g = 0, b = 0, a = 0;

  Color() = default;
  
  explicit Color(std::string_view name);

  template <std::convertible_to<uint8_t> Tr, std::convertible_to<uint8_t> Tg,
            std::convertible_to<uint8_t> Tb, std::convertible_to<uint8_t> Ta = int>
  constexpr Color(Tr _r, Tg _g, Tb _b, Ta _a = 255)
      : r(static_cast<uint8_t>(_r)), g(static_cast<uint8_t>(_g)),
        b(static_cast<uint8_t>(_b)), a(static_cast<uint8_t>(_a)) {}

  SDL_Color to_SDL_Color() const;
  /**return Uint32 kind of color*/
  uint32_t toUint32Color(uint32_t pixel_format = SDL_PIXELFORMAT_RGBA32) const;
  std::string toString();

  Color operator+(const Color &) const;
  Color operator-(const Color &) const;
  Color operator*(const Color &) const;
  Color operator/(const Color &) const;
  Color operator%(const Color &) const;

  Color operator~() const; // inverse color
};
} // namespace sdlgame::color

#endif
