#include "draw.hpp"
#include "color.hpp"
#include "display.hpp"
#include "engine.hpp"
#include "rect.hpp"
#include "surface.hpp"
#include <SDL_rect.h>

namespace sdlgame::draw {
void rect(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
          sdlgame::color::Color color, rect::Rect v_rect, int width) {
  std::cerr << "Called API\n";
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                                surface.getTexture()));
  SDL_CHECK(SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r,
                                   color.g, color.b, color.a));
  std::cerr << "Set target and color\n";
  if (width == 0) {
    SDL_FRect tmp = v_rect.to_SDL_FRect();
    SDL_CHECK(SDL_RenderFillRectF(sdlgame::display::get_renderer(), &tmp));
  } else if (width > 0) {
    SDL_FRect top =
        v_rect.inflate(0.0, width - v_rect.getHeight()).to_SDL_FRect();
    SDL_FRect left =
        v_rect.inflate(width - v_rect.getWidth(), 0.0).to_SDL_FRect();
    SDL_FRect bottom = v_rect.inflate(0.0, width - v_rect.getHeight())
                           .move(0.0, v_rect.getHeight() - width)
                           .to_SDL_FRect();
    SDL_FRect right = v_rect.inflate(width - v_rect.getWidth(), 0.0)
                          .move(v_rect.getWidth() - width, 0.0)
                          .to_SDL_FRect();
    SDL_CHECK(SDL_RenderFillRectF(sdlgame::display::get_renderer(), &top));
    SDL_CHECK(SDL_RenderFillRectF(sdlgame::display::get_renderer(), &left));
    SDL_CHECK(SDL_RenderFillRectF(sdlgame::display::get_renderer(), &bottom));
    SDL_CHECK(SDL_RenderFillRectF(sdlgame::display::get_renderer(), &right));
  }
  std::cerr << "Drawn\n";
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
}

void line(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
          sdlgame::color::Color color, double x1, double y1, double x2,
          double y2) {
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
          sdlgame::color::Color color, math::Vector2 start, math::Vector2 end) {
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                                surface.getTexture()));
  SDL_CHECK(SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r,
                                   color.g, color.b, color.a));

  SDL_CHECK(SDL_RenderDrawLineF(
      sdlgame::display::get_renderer(), static_cast<float>(start.x),
      static_cast<float>(start.y), static_cast<float>(end.x),
      static_cast<float>(end.y)));
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
}
void circle(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
            sdlgame::color::Color color, int centerX, int centerY, int radius,
            int width) {
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
      rad = next;
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
             const std::vector<math::Vector2> &points) {
  if (points.size() < 3)
    throw std::invalid_argument(
        "can't draw polygon with only 2 vertices or less");
  for (size_t i{0}; i < points.size() - 1; i++) {
    line(surface, color, points[i], points[i + 1]);
  }
  line(surface, color, points[0], points[points.size() - 1]);
}

void point(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
           sdlgame::color::Color color, double x, double y) {
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                                surface.getTexture()));
  SDL_CHECK(SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r,
                                   color.g, color.b, color.a));
  SDL_FPoint point = {static_cast<float>(x), static_cast<float>(y)};
  SDL_CHECK(SDL_RenderDrawPointsF(sdlgame::display::get_renderer(), &point, 1));
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
}

void points(sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &surface,
            sdlgame::color::Color color,
            const std::vector<math::Vector2> &v_points) {
  if (v_points.empty())
    return; 
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(),
                                surface.getTexture()));
  SDL_CHECK(SDL_SetRenderDrawColor(sdlgame::display::get_renderer(), color.r,
                                   color.g, color.b, color.a));
  std::vector<SDL_FPoint> sdl_points(v_points.size());
  for (size_t i = 0; i < v_points.size(); i++)
    sdl_points[i] = v_points[i].to_SDL_FPoint();
  SDL_CHECK(SDL_RenderDrawPointsF(sdlgame::display::get_renderer(),
                                  sdl_points.data(),
                                  static_cast<int>(v_points.size())));
  SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
}
} // namespace sdlgame::draw
