#pragma once
#include <optional>
#ifndef SDLGAME_TRANSFORM_
#define SDLGAME_TRANSFORM_

#include "display.hpp"
#include "engine.hpp"
#include "math.hpp"
#include "surface.hpp"
#include <SDL_render.h>
#include <algorithm>

namespace sdlgame::transform {

/**
 * @return a flipped image in certain axis
 * @param surface source surface
 * @param flip_x whether to flip x or not
 * @param flip_y whether to flip y or not
 */
template <SDL_TextureAccess From,
          SDL_TextureAccess To = SDL_TEXTUREACCESS_TARGET>
surface::Surface<To> flip(const surface::Surface<From> &surface, bool flip_x,
                          bool flip_y) {
  surface::Surface<SDL_TEXTUREACCESS_TARGET> res =
      surface.template copy<SDL_TEXTUREACCESS_TARGET>();
  SDL_CHECK(
      SDL_SetRenderTarget(sdlgame::display::get_renderer(), res.getTexture()));

  SDL_RendererFlip flipType = static_cast<SDL_RendererFlip>(
      SDL_FLIP_NONE | (static_cast<int>(flip_x) * SDL_FLIP_HORIZONTAL) |
      (static_cast<int>(flip_y) * SDL_FLIP_VERTICAL));

  SDL_CHECK(SDL_RenderCopyEx(sdlgame::display::get_renderer(),
                             surface.getTexture(), nullptr, nullptr, 0, nullptr,
                             flipType));
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));

  if constexpr (To == SDL_TEXTUREACCESS_TARGET) {
    return res;
  } else {
    return res.template copy<To>();
  }
}

template <SDL_TextureAccess From,
          SDL_TextureAccess To = SDL_TEXTUREACCESS_TARGET>
surface::Surface<To> scale(const surface::Surface<From> &surface,
                           math::Vector2 size) {
  surface::Surface<SDL_TEXTUREACCESS_TARGET> res(static_cast<int>(size.x),
                                                 static_cast<int>(size.y));
  SDL_CHECK(
      SDL_SetRenderTarget(sdlgame::display::get_renderer(), res.getTexture()));

  SDL_CHECK(SDL_RenderCopyF(sdlgame::display::get_renderer(),
                            surface.getTexture(), nullptr, nullptr));
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));

  if constexpr (To == SDL_TEXTUREACCESS_TARGET) {
    return res;
  } else {
    return res.template copy<To>();
  }
}

template <SDL_TextureAccess From,
          SDL_TextureAccess To = SDL_TEXTUREACCESS_TARGET>
surface::Surface<To> scale_by(const surface::Surface<From> &surface,
                              double factor) {
  return scale<From, To>(surface, surface.get_size() * factor);
}

/**
 * return a surface that rotated a certain angle counter-clockwise with passed
 * center angle unit is degrees
 */
template <SDL_TextureAccess From,
          SDL_TextureAccess To = SDL_TEXTUREACCESS_TARGET>
surface::Surface<To>
rotate(const surface::Surface<From> &surface, double angle_deg,
       std::optional<math::Vector2> o_center = std::nullopt) {
  math::Vector2 center = surface.get_size() / 2;
  if (o_center.has_value())
    center = o_center.value();

  math::Vector2 newtopleft =
      (surface.get_rect().getTopLeft() - center).rotate(angle_deg);
  math::Vector2 newbotleft =
      (surface.get_rect().getBottomLeft() - center).rotate(angle_deg);
  math::Vector2 newtopright =
      (surface.get_rect().getTopRight() - center).rotate(angle_deg);
  math::Vector2 newbotright =
      (surface.get_rect().getBottomRight() - center).rotate(angle_deg);

  surface::Surface<SDL_TEXTUREACCESS_TARGET> res(
      static_cast<int>(std::ranges::max({newtopleft.x, newbotleft.x,
                                         newbotright.x, newtopright.x}) -
                       std::ranges::min({newtopleft.x, newbotleft.x,
                                         newbotright.x, newtopright.x})),
      static_cast<int>(std::ranges::max({newtopleft.y, newbotleft.y,
                                         newbotright.y, newtopright.y}) -
                       std::ranges::min({newtopleft.y, newbotleft.y,
                                         newbotright.y, newtopright.y})));

  SDL_CHECK(
      SDL_SetRenderTarget(sdlgame::display::get_renderer(), res.getTexture()));

  SDL_FPoint tmp = {float(center.x), float(center.y)};
  SDL_CHECK(
      SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), 0, 0, 0, 0));
  SDL_CHECK(SDL_RenderClear(sdlgame::display::get_renderer()));
  SDL_CHECK(SDL_RenderCopyExF(sdlgame::display::get_renderer(),
                              surface.getTexture(), nullptr, nullptr, angle_deg,
                              &tmp, SDL_FLIP_NONE));
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));

  if constexpr (To == SDL_TEXTUREACCESS_TARGET) {
    return res;
  } else {
    return res.template copy<To>();
  }
}

} // namespace sdlgame::transform

#endif
