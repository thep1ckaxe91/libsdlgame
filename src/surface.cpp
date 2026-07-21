#include "surface.hpp"
#include "color.hpp"
#include "display.hpp"
#include "math.hpp"
#include "memory.hpp"
#include "rect.hpp"
#include <SDL_blendmode.h>
#include <SDL_error.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <cpptrace/cpptrace.hpp>
#include <iostream>
#include <utility>
#include <exception>
/**
 * @brief clear, and then copy texture content, this has no resposibility to
 manager the underlying memory

 if from is nullptr, only clear the dest texture
 *
 * @param from source texture
 * @param to destination texture
 */
static void copy_texture(SDL_Texture *from,
                         const sdlgame::memory::SDLUniquePtr<SDL_Texture> &to) {
  using namespace sdlgame;

  SDL_SetTextureBlendMode(to.get(), SDL_BLENDMODE_NONE);
  SDL_SetRenderTarget(display::get_renderer(), to.get());
  SDL_SetRenderDrawColor(display::get_renderer(), 0, 0, 0, 0);
  SDL_RenderClear(display::get_renderer());
  if (from != nullptr)
    SDL_RenderCopy(display::get_renderer(), from, nullptr, nullptr);
  SDL_SetRenderTarget(display::get_renderer(), nullptr);
}

namespace sdlgame::surface {

Surface::Surface(int width, int height) {
  texture.reset(SDL_CreateTexture(display::get_renderer(),
                                  SDL_PIXELFORMAT_RGBA32, SURFACE_TYPE, width,
                                  height));

  if (!texture) {
    std::cerr << "Failed to create texture\nErr: " << SDL_GetError() << '\n';
    std::terminate();
  }

  size.x = width;
  size.y = height;
  copy_texture(nullptr, texture);
}

Surface::Surface(const Surface &oth) {
  int w, h;

  if (SDL_QueryTexture(oth.getTexture(), nullptr, nullptr, &w, &h))
      [[unlikely]] {
    std::cerr << "Failed to query copy target texture\nErr: " << SDL_GetError() << '\n';
    std::terminate();
  }

  texture.reset(SDL_CreateTexture(display::get_renderer(),
                                  SDL_PIXELFORMAT_RGBA32, SURFACE_TYPE, w, h));
  if (texture == nullptr) [[unlikely]] {
    std::cerr << "Failed to create texture from another Surface object\nErr: " << SDL_GetError() << '\n';
    std::terminate();
  }
  copy_texture(oth.getTexture(), texture);
  size.x = w;
  size.y = h;
}

Surface::Surface(Surface &&other) noexcept
    : texture(std::move(other.texture)),
      size(std::exchange(other.size, {0, 0})) {}

Surface::Surface(SDL_Texture *oth) {
  sdlgame::memory::SDLUniquePtr<SDL_Texture> old_tex{oth};
  int w, h;
  SDL_QueryTexture(old_tex.get(), nullptr, nullptr, &w, &h);
  texture.reset(SDL_CreateTexture(display::get_renderer(),
                                  SDL_PIXELFORMAT_RGBA32, SURFACE_TYPE, w, h));
  if (texture == nullptr) {
    std::cerr << "Failed to create texture from another texture\nErr: " << SDL_GetError() << '\n';
    std::terminate();
  }

  copy_texture(oth, texture);

  size.x = w;
  size.y = h;
}

Surface::Surface(SDL_Surface *surf) : size(surf->w, surf->h) {
  memory::SDLUniquePtr<SDL_Texture> s_tex{
      SDL_CreateTextureFromSurface(display::get_renderer(), surf)};
  // FIXME: the return texture return by create texture from surface is static

  texture.reset(SDL_CreateTexture(sdlgame::display::get_renderer(),
                                  SDL_PIXELFORMAT_RGBA32, SURFACE_TYPE, surf->w,
                                  surf->h));

  if (!texture) {
    std::cerr << "Failed to create new texture while creating from surface\nErr:" << SDL_GetError() << '\n';
    std::terminate();
  }

  copy_texture(s_tex.get(), texture);
}

Surface &Surface::operator=(const Surface &other) {
  if (!other.texture) [[unlikely]] {
    if (texture) {
      std::cout << "Warning: Copy a null texture.\n";
      texture.reset();
    }
    size = other.size;
  } else if (this != &other) [[likely]] {
    if (texture != nullptr)
      texture.reset();

    int w, h;
    SDL_QueryTexture(other.getTexture(), nullptr, nullptr, &w, &h);

    auto new_tex = SDL_CreateTexture(
        display::get_renderer(), SDL_PIXELFORMAT_RGBA32, SURFACE_TYPE, w, h);
    if (new_tex == nullptr) [[unlikely]] {
      std::cerr << "Failed to create texture which assigning\nErr: " << SDL_GetError() << '\n';
      std::terminate();
    }
    texture.reset(new_tex);

    SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(display::get_renderer(), texture.get());
    SDL_SetRenderDrawColor(display::get_renderer(), 0, 0, 0, 0);
    SDL_RenderClear(display::get_renderer());
    SDL_RenderCopy(display::get_renderer(), other.getTexture(), nullptr,
                   nullptr);
    SDL_SetRenderTarget(display::get_renderer(), nullptr);
    size = other.size;
  }
  return *this;
}

Surface &Surface::operator=(Surface &&other) noexcept(true) {
  if (this != std::addressof(other)) {
    texture.reset(other.texture.release());
    size = other.size;
  }
  return *this;
}

/**
 * Return a copy of the surface rect
 *
 */
rect::Rect Surface::get_rect() const {
  return rect::Rect(0, 0, size.x, size.y);
}
SDL_Texture *Surface::getTexture() const { return texture.get(); }
/**
 * Blit a surface onto this surface with position and size, leave size be -1,-1
will be its original size
 * the surface or image will stretch or shrink acoording to the size
 */
void Surface::blit(const Surface &source, math::Vector2 pos,
                   math::Vector2 _size, rect::Rect area) {
  if (area == rect::Rect()) {
    area = rect::Rect(0, 0, source.get_width(), source.get_height());
  }
  rect::Rect destrect =
      rect::Rect(pos.x, pos.y, (_size.x < 0 ? source.get_width() : _size.x),
                 (_size.y < 0 ? source.get_height() : _size.y));

  if (SDL_SetRenderTarget(display::get_renderer(), texture.get())) {
    std::cerr << "Failed to set target when blit:\nTexture: " << (void *)texture.get() << "\nError: " << SDL_GetError() << '\n';
    std::terminate();
  }

  SDL_Rect srcrect = area.to_SDL_Rect();
  SDL_FRect dstrect = destrect.to_SDL_FRect();

  if (SDL_RenderCopyF(display::get_renderer(), source.getTexture(), &srcrect,
                      &dstrect)) {
    std::cerr << "Error copy texture onto another\n" << SDL_GetError() << '\n';
    std::terminate();
  }

  if (SDL_SetRenderTarget(display::get_renderer(), nullptr)) {
    std::cerr << "Failed to reset target when blit: " << SDL_GetError() << '\n';
    std::terminate();
  }
}
void Surface::fill(sdlgame::color::Color color) {
  if (SDL_SetRenderTarget(display::get_renderer(), texture.get())) {
    std::cerr << "Failed to set target when fill:\nTexture: " << (void *)texture.get() << "\nError: " << SDL_GetError() << '\n';
    cpptrace::generate_trace().print();
    std::terminate();
  }

  if (SDL_SetRenderDrawColor(display::get_renderer(), color.r, color.g, color.b,
                             color.a)) {
    std::cerr << "Cannot set renderer draw color before fill surface \nErr: " << SDL_GetError() << '\n';
    std::terminate();
  }
  if (SDL_RenderClear(display::get_renderer())) {
    std::cerr << "Cannot perform fill on surface\nErr: " << SDL_GetError() << '\n';
    std::terminate();
  }

  if (SDL_SetRenderTarget(display::get_renderer(), nullptr)) {
    std::cerr << "Failed to reset target when fill: " << SDL_GetError() << '\n';
    std::terminate();
  }
}
math::Vector2 Surface::get_size() const { return size; }
double Surface::get_width() const { return size.x; }
double Surface::get_height() const { return size.y; }

} // namespace sdlgame::surface
