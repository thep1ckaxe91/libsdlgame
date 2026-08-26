#include "transform.hpp"
#include "display.hpp"
#include <algorithm>
#include <iostream>
#include <exception>

namespace sdlgame::transform {

surface::Surface flip(const surface::Surface &surface, bool flip_x,
                      bool flip_y) {
  surface::Surface res = surface;
  if (auto ec = SDL_SetRenderTarget(sdlgame::display::get_renderer(), res.getTexture()); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when flip:\nTexture: " << (void *)res.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_RendererFlip flipType = static_cast<SDL_RendererFlip>(
      SDL_FLIP_NONE | (static_cast<int>(flip_x) * SDL_FLIP_HORIZONTAL) |
      (static_cast<int>(flip_y) * SDL_FLIP_VERTICAL));
  if (auto ec = SDL_RenderCopyEx(sdlgame::display::get_renderer(), surface.getTexture(),
                       nullptr, nullptr, 0, nullptr, flipType); ec != 0) [[unlikely]] {
    std::cerr << "Failed to flip\nError code: " << ec << '\n';
    std::terminate();
  }
  if (auto ec = SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when flip: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  return res;
}
surface::Surface scale(const surface::Surface &surface, math::Vector2 size) {
  surface::Surface res = surface::Surface(static_cast<int>(size.x), static_cast<int>(size.y));
  if (auto ec = SDL_SetRenderTarget(sdlgame::display::get_renderer(), res.getTexture()); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when scale:\nTexture: " << (void *)res.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_RenderCopyF(sdlgame::display::get_renderer(), surface.getTexture(),
                  nullptr, nullptr);
  if (auto ec = SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when scale: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  return res;
}

surface::Surface scale_by(const surface::Surface &surface, double factor) {
  return scale(surface, surface.get_size() * factor);
}

/**
 * return a surface that rotated a certain angle counter-clokcwise with passed
 * center angle unit is degrees
 */
surface::Surface rotate(const surface::Surface &surface, double angle_deg,
                        math::Vector2 center) {
  math::Vector2 newtopleft =
      (surface.get_rect().getTopLeft() - center).rotate(angle_deg);
  math::Vector2 newbotleft =
      (surface.get_rect().getBottomLeft() - center).rotate(angle_deg);
  math::Vector2 newtopright =
      (surface.get_rect().getTopRight() - center).rotate(angle_deg);
  math::Vector2 newbotright =
      (surface.get_rect().getBottomRight() - center).rotate(angle_deg);

  surface::Surface res =
      surface::Surface(static_cast<int>(std::ranges::max({newtopleft.x, newbotleft.x,
                                         newbotright.x, newtopright.x}) -
                           std::ranges::min({newtopleft.x, newbotleft.x,
                                             newbotright.x, newtopright.x})),
                       static_cast<int>(std::ranges::max({newtopleft.y, newbotleft.y,
                                         newbotright.y, newtopright.y}) -
                           std::ranges::min({newtopleft.y, newbotleft.y,
                                             newbotright.y, newtopright.y})));

  if (auto ec = SDL_SetRenderTarget(sdlgame::display::get_renderer(), res.getTexture()); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when rotate:\nTexture: " << (void *)res.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }


  SDL_FPoint tmp = {float(center.x), float(center.y)};
  SDL_RenderCopyExF(sdlgame::display::get_renderer(), surface.getTexture(),
                    nullptr, nullptr, angle_deg, &tmp, SDL_FLIP_NONE);
  if (auto ec = SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when rotate: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  return res;
}
} // namespace sdlgame::transform