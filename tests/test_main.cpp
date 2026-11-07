#include <gtest/gtest.h>
#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

int main(int argc, char **argv) {
  SDL_SetMainReady();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
