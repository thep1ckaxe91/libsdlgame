#include "draw.hpp"
#include "color.hpp"
#include "display.hpp"
#include "rect.hpp"
#include <iostream>
#include <exception>
#include "surface.hpp"
#include <SDL_rect.h>

namespace sdlgame::draw {
void rect(sdlgame::surface::Surface &surface, sdlgame::color::Color color,
          rect::Rect rect, int width) {
  // std::cout << surface.texture << " color: "<<color.toString() << " rect:
  // "<<rect.toString()<<std::endl;
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                          surface.getTexture())); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when draw rect:\nTexture: " << (void *)surface.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r, color.g,
                         color.b, color.a);

  if (width == 0) {
    SDL_FRect tmp = rect.to_SDL_FRect();
    if (auto ec = (SDL_RenderFillRectF(sdlgame::display::get_renderer(), &tmp)); ec != 0) [[unlikely]] {
      std::cerr << "Error filling a rectangle: " << SDL_GetError() << "\nError code: " << ec << '\n';
      std::terminate();
    }
  } else if (width > 0) {
    SDL_FRect top = rect.inflate(0.0, width - rect.getHeight()).to_SDL_FRect();
    SDL_FRect left = rect.inflate(width - rect.getWidth(), 0.0).to_SDL_FRect();
    SDL_FRect bottom = rect.inflate(0.0, width - rect.getHeight())
                           .move(0.0, rect.getHeight() - width)
                           .to_SDL_FRect();
    SDL_FRect right = rect.inflate(width - rect.getWidth(), 0.0)
                          .move(rect.getWidth() - width, 0.0)
                          .to_SDL_FRect();
    if (auto ec = (SDL_RenderFillRectF(sdlgame::display::get_renderer(), &top) or
        SDL_RenderFillRectF(sdlgame::display::get_renderer(), &left) or
        SDL_RenderFillRectF(sdlgame::display::get_renderer(), &bottom) or
        SDL_RenderFillRectF(sdlgame::display::get_renderer(), &right)); ec != 0) [[unlikely]] {
      std::cerr << "Error drawing a rectangle: " << SDL_GetError() << "\nError code: " << ec << '\n';
      std::terminate();
    }
  }
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr)); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when draw rect: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
}

void line(sdlgame::surface::Surface &surface, sdlgame::color::Color color,
          double x1, double y1, double x2, double y2) {
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                          surface.getTexture())); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when draw line:\nTexture: " << (void *)surface.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r, color.g,
                         color.b, color.a);

  if (auto ec = (SDL_RenderDrawLineF(sdlgame::display::get_renderer(), static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2))); ec != 0) [[unlikely]] {
    std::cerr << "Failed to draw a line: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr)); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when draw line: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
}
void line(sdlgame::surface::Surface &surface, sdlgame::color::Color color,
          math::Vector2 start, math::Vector2 end) {
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                          surface.getTexture())); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when draw line:\nTexture: " << (void *)surface.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r, color.g,
                         color.b, color.a);

  if (auto ec = (SDL_RenderDrawLineF(sdlgame::display::get_renderer(), static_cast<float>(start.x), static_cast<float>(start.y),
                          static_cast<float>(end.x), static_cast<float>(end.y))); ec != 0) [[unlikely]] {
    std::cerr << "Failed to draw a line: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr)); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when draw line: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
}
void circle(sdlgame::surface::Surface &surface, sdlgame::color::Color color,
            int centerX, int centerY, int radius, int width) {
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                          surface.getTexture())); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when draw circle:\nTexture: " << (void *)surface.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r, color.g,
                         color.b, color.a);

  if (width != 0) {
    int quality = 90;
    math::Vector2 rad(radius, 0);
    for (int i = 0; i <= quality; i++) {
      math::Vector2 next = rad.rotate(360.0 / static_cast<double>(quality));
      SDL_RenderDrawLineF(sdlgame::display::get_renderer(), static_cast<float>(centerX + rad.x),
                          static_cast<float>(centerY + rad.y), static_cast<float>(centerX + next.x), static_cast<float>(centerY + next.y));
      rad = next;
    }
  } else {
    double x;
    for (int i = -radius; i <= radius; i++) {
      x = radius * std::cos(std::asin(i * 1.0 / radius));
      SDL_RenderDrawLineF(sdlgame::display::get_renderer(), static_cast<float>(x + centerX),
                          static_cast<float>(i + centerY), static_cast<float>(centerX - x), static_cast<float>(i + centerY));
    }
  }
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr)); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when draw circle: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
}
void polygon(sdlgame::surface::Surface &surface, sdlgame::color::Color color,
             std::vector<math::Vector2> points) {
  if (points.size() < 3)
    throw std::invalid_argument(
        "can't draw polygon with only 2 vertices or less");
  for (int i = 0; i < int(points.size()) - 1; i++) {
    line(surface, color, points[i], points[i + 1]);
  }
  line(surface, color, points[0], points[points.size() - 1]);
}

void point(sdlgame::surface::Surface &surface, sdlgame::color::Color color,
           double x, double y) {
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                          surface.getTexture())); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when draw point:\nTexture: " << (void *)surface.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r, color.g,
                         color.b, color.a);
  SDL_FPoint point = {static_cast<float>(x), static_cast<float>(y)};
  if (auto ec = (SDL_RenderDrawPointsF(sdlgame::display::get_renderer(), &point, 1)); ec != 0) [[unlikely]] {
    std::cerr << "Failed to draw a point: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr)); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when draw point: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
}

void points(sdlgame::surface::Surface &surface, sdlgame::color::Color color,
            const std::vector<math::Vector2> &points) {
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                          surface.getTexture())); ec != 0) [[unlikely]] {
    std::cerr << "Failed to set target when draw points:\nTexture: " << (void *)surface.getTexture() << "\nError: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r, color.g,
                         color.b, color.a);
  std::vector<SDL_FPoint> sdl_points(points.size());
  for (size_t i = 0; i < points.size(); i++)
    sdl_points[i] = points[i].to_SDL_FPoint();
  if (auto ec = (SDL_RenderDrawPointsF(sdlgame::display::get_renderer(), sdl_points.data(),
                            static_cast<int>(points.size()))); ec != 0) [[unlikely]] {
    std::cerr << "Failed to draw points: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
  if (auto ec = (SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr)); ec != 0) [[unlikely]] {
    std::cerr << "Failed to reset target when draw points: " << SDL_GetError() << "\nError code: " << ec << '\n';
    std::terminate();
  }
}
} // namespace sdlgame::draw