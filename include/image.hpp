#pragma once
#include <SDL_render.h>
#ifndef SDLGAME_IMAGE_
#define SDLGAME_IMAGE_
#include "surface.hpp"
#include <filesystem>
namespace sdlgame::image {
/**
 * Currently only support JPG and PNG type
 */
void init();
/**
 * load an image from file path, require you to create the window object first
 */
surface::Surface<SDL_TEXTUREACCESS_STATIC>
load(const std::filesystem::path &path);
} // namespace sdlgame::image

#endif