#pragma once
#ifndef SDLGAME_SURFACE_
#define SDLGAME_SURFACE_
#include "color.hpp"
#include "math.hpp"
#include "memory.hpp"
#include "rect.hpp"
#include "engine.hpp"
#include "display.hpp"
#include <stdexcept>
#include <optional>
#include <algorithm>

namespace sdlgame::surface {

inline Uint32 optimal_format() { return SDL_PIXELFORMAT_RGBA32; }

template <SDL_TextureAccess AccessPattern = SDL_TEXTUREACCESS_TARGET>
class Surface {
private:
  sdlgame::memory::SDLUniquePtr<SDL_Texture> texture;
  struct { void* pixels = nullptr; int pitch = 0; } state;

public:
  template <SDL_TextureAccess Other> friend class Surface;
  math::Vector2 size;

  Surface() = default;
  Surface(Surface &&) noexcept = default;
  Surface(int width, int height) {
    auto tex = SDL_NEW(SDL_CreateTexture(sdlgame::display::get_renderer(),
                                         optimal_format(), AccessPattern, width,
                                         height));
    texture.reset(tex);
    size = {static_cast<double>(width), static_cast<double>(height)};
  }

  Surface(const Surface &) = delete;
  Surface &operator=(const Surface &) = delete;
  Surface &operator=(Surface &&) noexcept = default;

  explicit Surface(SDL_Surface *surf) {
    auto tex = SDL_NEW(SDL_CreateTextureFromSurface(sdlgame::display::get_renderer(), surf));
    texture.reset(tex);
    size = {static_cast<double>(surf->w), static_cast<double>(surf->h)};
  }

  explicit Surface(SDL_Texture *tex) {
    Uint32 format;
    int access;
    SDL_QueryTexture(tex, &format, &access, NULL, NULL);
    if (access != AccessPattern) [[unlikely]] {
      throw std::invalid_argument("The texture doesn't have the given access pattern");
    }
    texture.reset(tex);
    int w, h;
    SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
    size = {static_cast<double>(w), static_cast<double>(h)};
  }

  template <SDL_TextureAccess OtherAccess>
  void blit(const Surface<OtherAccess> &source, math::Vector2 pos,
            std::optional<math::Vector2> source_size = std::nullopt,
            std::optional<rect::Rect> area = std::nullopt) {
    static_assert(AccessPattern == SDL_TEXTUREACCESS_TARGET,
                  "Cannot blit to a non-TARGET surface.");
    SDL_Rect srcrect =
        area.value_or(rect::Rect(0, 0, source.get_width(), source.get_height()))
            .to_SDL_Rect();
    SDL_FRect dstrect =
        sdlgame::rect::Rect(pos, source_size.value_or(math::Vector2(
                                     source.get_width(), source.get_height())))
            .to_SDL_FRect();
    SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), texture.get()));
    SDL_RenderCopyF(sdlgame::display::get_renderer(), source.getTexture(),
                    &srcrect, &dstrect);
    SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
  }

  void fill(sdlgame::color::Color color) {
    static_assert(AccessPattern != SDL_TEXTUREACCESS_STATIC,
                  "Can't fill on a static texture");
    if constexpr (AccessPattern == SDL_TEXTUREACCESS_STREAMING) {
      void *pixels;
      int pitch;
      SDL_CHECK(SDL_LockTexture(texture.get(), nullptr, &pixels, &pitch));
      int h = get_height();
      Uint32 *p = static_cast<Uint32 *>(pixels);
      std::ranges::fill(p, p + ((pitch / static_cast<int>(sizeof(Uint32))) * h),
                        color.toUint32Color());
      SDL_UnlockTexture(texture.get());
    }
    if constexpr (AccessPattern == SDL_TEXTUREACCESS_TARGET) {
      SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), texture.get()));
      SDL_CHECK(SDL_SetRenderDrawColor(sdlgame::display::get_renderer(),
                                       color.r, color.g, color.b, color.a));
      SDL_CHECK(SDL_RenderClear(sdlgame::display::get_renderer()));
      SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
    }
  }

  void lock() requires(AccessPattern == SDL_TEXTUREACCESS_STREAMING) {
    SDL_CHECK(SDL_LockTexture(texture.get(), nullptr, &state.pixels, &state.pitch));
  }

  std::tuple<void *, int> get_lock() const requires(AccessPattern == SDL_TEXTUREACCESS_STREAMING) {
    return {state.pixels, state.pitch};
  }

  void unlock() requires(AccessPattern == SDL_TEXTUREACCESS_STREAMING) {
    state.pixels = nullptr;
    state.pitch = 0;
    SDL_UnlockTexture(texture.get());
  }

  template <SDL_TextureAccess To> Surface<To> copy() const {
    Uint32 format;
    int w, h;
    SDL_CHECK(SDL_QueryTexture(texture.get(), &format, nullptr, &w, &h));

    if constexpr (To == SDL_TEXTUREACCESS_TARGET) {
      SDL_Texture *targetTexture = SDL_NEW(SDL_CreateTexture(sdlgame::display::get_renderer(), format, SDL_TEXTUREACCESS_TARGET, w, h));
      SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), targetTexture));
      SDL_CHECK(SDL_RenderCopy(sdlgame::display::get_renderer(), texture.get(), nullptr, nullptr));
      SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));
      auto res = Surface<To>{};
      res.texture.reset(targetTexture);
      res.size = {static_cast<double>(w), static_cast<double>(h)};
      return res;
    }

    SDL_Texture *staging_target = nullptr;
    if constexpr (AccessPattern == SDL_TEXTUREACCESS_TARGET) {
      staging_target = texture.get();
    } else {
      staging_target = SDL_NEW(SDL_CreateTexture(sdlgame::display::get_renderer(), format, SDL_TEXTUREACCESS_TARGET, w, h));
      SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), staging_target));
      SDL_CHECK(SDL_RenderCopy(sdlgame::display::get_renderer(), texture.get(), nullptr, nullptr));
    }

    sdlgame::memory::SDLUniquePtr<SDL_Surface> cpu_buffer;
    cpu_buffer.reset(SDL_NEW(SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, format)));
    SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), staging_target));
    SDL_CHECK(SDL_RenderReadPixels(sdlgame::display::get_renderer(), nullptr, format, cpu_buffer->pixels, cpu_buffer->pitch));
    SDL_CHECK(SDL_SetRenderTarget(sdlgame::display::get_renderer(), nullptr));

    if constexpr (AccessPattern != SDL_TEXTUREACCESS_TARGET) {
      SDL_DestroyTexture(staging_target);
    }

    SDL_Texture *final_texture = SDL_NEW(SDL_CreateTexture(sdlgame::display::get_renderer(), format, To, w, h));
    SDL_CHECK(SDL_UpdateTexture(final_texture, nullptr, cpu_buffer->pixels, cpu_buffer->pitch));

    auto res = Surface<To>{};
    res.texture.reset(final_texture);
    res.size = {static_cast<double>(w), static_cast<double>(h)};
    return res;
  }

  math::Vector2 get_size() const { return size; }
  double get_width() const { return size.x; }
  double get_height() const { return size.y; }
  SDL_Texture *getTexture() const { return texture.get(); }
  rect::Rect get_rect() const { return rect::Rect(0, 0, size.x, size.y); }
  ~Surface() = default;
};

} // namespace sdlgame::surface

#endif
