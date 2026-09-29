#include "key.hpp"

#include "SDL2/SDL_keyboard.h"
#include <SDL.h>
#include <cassert>

#include "engine.hpp"

namespace sdlgame::key {
static int numKeys = 0;
static const uint8_t *keyState = nullptr;

void init() {
  SDL_CHECK(-(SDL_WasInit(0) == 0));
  keyState = SDL_NEW(SDL_GetKeyboardState(&numKeys));
}

std::span<const uint8_t> get_pressed() {
  assert(keyState != nullptr && "sdlgame::key::init() was never called\n");
  return {keyState, static_cast<size_t>(numKeys)};
}
} // namespace sdlgame::key