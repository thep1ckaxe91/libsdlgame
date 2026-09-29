#include "display.hpp"
#include "SDL2/SDL_hints.h"
#include "SDL2/SDL_image.h"
#include "engine.hpp"
#include "math.hpp"
#include "memory.hpp"
#include "surface.hpp"
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <exception>
#include <iostream>

namespace sdlgame::display {
namespace {
sdlgame::memory::SDLUniquePtr<SDL_Window> window = nullptr;
sdlgame::memory::SDLUniquePtr<SDL_Renderer> renderer = nullptr;

sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> proxy_surf;
math::Vector2 resolution;
} // namespace

sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &
set_mode(int width, int height, uint32_t flags) {
  if (width == 0 || height == 0) {
    SDL_DisplayMode DM;
    SDL_CHECK(SDL_GetDesktopDisplayMode(0, &DM));
    width = DM.w;
    height = DM.h;
  }
  if(width < 0 || height < 0 || width > (1 << 14) || height > (1 << 14)) {
    std::cerr << "Can't initialize window with negative/too large size\n";
    std::terminate();
  }

  resolution = math::Vector2(width, height);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

  window.reset(
      SDL_NEW(SDL_CreateWindow("SDLgame", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height, flags)));

  renderer.reset(SDL_NEW(SDL_CreateRenderer(
      window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)));

  SDL_CHECK(SDL_RenderSetLogicalSize(renderer.get(), width, height));

  // Fulfill the Pygame syntax requirement
  proxy_surf.size = math::Vector2(width, height);
  return proxy_surf;
}

bool set_render_scale_quality(bool linear) {
  if (linear)
    return SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "linear",
                                   SDL_HINT_OVERRIDE);
  return SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "nearest",
                                 SDL_HINT_OVERRIDE);
}
void maximize() {
  SDL_CHECK(SDL_SetWindowFullscreen(window.get(), 0));
  SDL_MaximizeWindow(window.get());
}

void minimize() {
  SDL_CHECK(SDL_SetWindowFullscreen(window.get(), 0));
  SDL_MinimizeWindow(window.get());
}
void restore() {
  SDL_CHECK(SDL_SetWindowFullscreen(window.get(), 0));
  SDL_RestoreWindow(window.get());
}
void fullscreen() {
  SDL_CHECK(SDL_SetWindowFullscreen(window.get(), SDL_WINDOW_FULLSCREEN));
}
bool is_fullscreen() {
  return (SDL_GetWindowFlags(window.get()) & SDL_WINDOW_FULLSCREEN_DESKTOP) ||
         (SDL_GetWindowFlags(window.get()) & SDL_WINDOW_FULLSCREEN);
}
void set_window_size(int w, int h) {
  SDL_SetWindowSize(window.get(), w, h);
  proxy_surf.size = {static_cast<double>(w), static_cast<double>(h)};
}
// set position of window, use sdlgame::WINDOWPOS_CENTERED if you need center
void set_window_pos(int x, int y) { SDL_SetWindowPosition(window.get(), x, y); }
std::pair<int, int> get_window_pos() {
  int x, y;
  SDL_GetWindowPosition(window.get(), &x, &y);
  return {x, y};
}
math::Vector2 get_window_size() {
  int w, h;
  SDL_GetWindowSize(window.get(), &w, &h);
  // SDL_GetWindowSurface(window.get());
  return proxy_surf.size = math::Vector2(w, h);
}

void fullscreen_desktop() {
  SDL_CHECK(
      SDL_SetWindowFullscreen(window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP));
}
sdlgame::surface::Surface<SDL_TEXTUREACCESS_TARGET> &get_surf() {
  return proxy_surf;
}

double get_width() {
  if (proxy_surf.get_width() == 0) {
    std::cerr << "Display not yet set mode\n";
    std::terminate();
  }
  return proxy_surf.get_width();
}
double get_height() {
  if (proxy_surf.get_height() == 0) {
    std::cerr << "Display not yet set mode\n";
    std::terminate();
  }
  return proxy_surf.get_height();
}
/**
 *  if set to true, the mouse will be confine to the window
 * this function get or set the state of mouse being confine or not
 *
 */
bool grab(int enable) {
  if (enable == -1)
    return SDL_GetWindowGrab(window.get());
  SDL_SetWindowGrab(window.get(), static_cast<SDL_bool>(enable));
  return enable;
}

void set_icon(const fs::path &icon_path) {
  sdlgame::memory::SDLUniquePtr<SDL_Surface> icon(
      SDL_NEW(IMG_Load(icon_path.string().c_str())));
  SDL_SetWindowIcon(window.get(), icon.get());
}

/**
 *  get and set the borderless state of the active window;
 */
bool borderless(int enable) {
  if (enable == -1)
    return (SDL_GetWindowFlags(window.get()) & SDL_WINDOW_BORDERLESS);
  SDL_SetWindowBordered(window.get(), (enable ? SDL_FALSE : SDL_TRUE));
  return (SDL_GetWindowFlags(window.get()) & SDL_WINDOW_BORDERLESS) > 0;
}
void set_caption(const std::string &title) {
  SDL_SetWindowTitle(window.get(), title.c_str());
}
SDL_Window *get_window() { return window.get(); }
SDL_Renderer *get_renderer() { return renderer.get(); }
void quit() {
  window.reset();
  renderer.reset();
}
void flip() {
  SDL_CHECK(SDL_SetRenderTarget(renderer.get(), nullptr));
  SDL_RenderPresent(renderer.get());
}
} // namespace sdlgame::display