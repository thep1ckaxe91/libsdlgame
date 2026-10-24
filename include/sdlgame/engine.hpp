#pragma once
#include <SDL_render.h>
#include <filesystem>
#include <iostream>
#ifndef ENGINE_HPP
#define ENGINE_HPP

namespace fs = std::filesystem;

namespace sdlgame {
namespace internal {
template <typename T>
inline T check_sdl_ptr(T ptr, const char *expr_str, const char *file,
                       int line) {
  if (ptr == nullptr) [[unlikely]] {
    std::cerr << "FATAL: SDL Error at " << file << ":" << line << '\n'
              << "Failing to Create Resource at: " << expr_str << '\n'
              << "SDL_GetError: " << SDL_GetError() << '\n';
    std::terminate();
  }
  return ptr;
}

template <typename T>
inline T check_sdl(T err_code, const char *expr_str, const char *file,
                     int line) {
  if (err_code < 0) [[unlikely]] {
    std::cerr << "FATAL: SDL Error at " << file << ":" << line << '\n'
              << "Failing Expression: " << expr_str << '\n'
              << "SDL_GetError: " << SDL_GetError() << '\n';
    std::terminate();
  }
  return err_code;
}
} // namespace internal

/**
 * @return base path to the exe file that call this function
 */
fs::path get_base_path();
void init();
void quit();
} // namespace sdlgame

#define SDL_NEW(expr)                                                          \
  sdlgame::internal::check_sdl_ptr((expr), #expr, __FILE__, __LINE__) // Check if expression return nullptr, if does: terminate
#define SDL_CHECK(expr)                                                        \
  sdlgame::internal::check_sdl((expr), #expr, __FILE__, __LINE__) // Check if expression return non-zero, if does: terminate

#endif
