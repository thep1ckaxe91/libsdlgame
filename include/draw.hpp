#pragma once
#include <SDL_render.h>
#ifndef SDLGAME_DRAW_
#define SDLGAME_DRAW_
#include "color.hpp"
#include "rect.hpp"
#include "surface.hpp"

namespace sdlgame::draw {
/**
 * width determine how far the border will expand to the INSIDE
 */
void rect(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
          sdlgame::color::Color color, rect::Rect rect, int width = 0);

void line(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
          sdlgame::color::Color color, Arithmetic auto x1, Arithmetic auto y1,
          Arithmetic auto x2, Arithmetic auto y2) {
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                                surface.getTexture()));
  SDL_CHECK(SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r,
                                   color.g, color.b, color.a));

  SDL_CHECK(SDL_RenderDrawLineF(
      sdlgame::display::get_renderer(), static_cast<float>(x1),
      static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2)));
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
}

void line(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
          sdlgame::color::Color color, math::Vector2 start, math::Vector2 end);

void circle(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
            sdlgame::color::Color color, Arithmetic auto centerX,
            Arithmetic auto centerY, Arithmetic auto radius, int width = 0) {
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                                surface.getTexture()));
  SDL_CHECK(SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r,
                                   color.g, color.b, color.a));

  if (width != 0) {
    int quality = 90;
    math::Vector2 rad(radius, 0);
    for (int i = 0; i <= quality; i++) {
      math::Vector2 next = rad.rotate(360.0 / static_cast<double>(quality));
      SDL_CHECK(SDL_RenderDrawLineF(sdlgame::display::get_renderer(),
                                    static_cast<float>(centerX + rad.x),
                                    static_cast<float>(centerY + rad.y),
                                    static_cast<float>(centerX + next.x),
                                    static_cast<float>(centerY + next.y)));
      rad = next; // TODO: thep1ckaxe - this can be more accurate by calculate base on the degree instead of using previous result
    }
  } else {
    for (int i = -radius; i <= radius; i++) {
      double x = radius * std::cos(std::asin(i * 1.0 / radius));
      SDL_CHECK(SDL_RenderDrawLineF(
          sdlgame::display::get_renderer(), static_cast<float>(x + centerX),
          static_cast<float>(i + centerY), static_cast<float>(centerX - x),
          static_cast<float>(i + centerY)));
    }
  }
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
}
void polygon(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
             sdlgame::color::Color color,
             const std::vector<math::Vector2> &points);
void point(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
           sdlgame::color::Color color, double x, double y);
void points(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
            sdlgame::color::Color color,
            const std::vector<math::Vector2> &points);
} // namespace sdlgame::draw
#endif