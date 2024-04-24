#include "core/window.hpp"

#include "gl3/gl3_sdl3_window.hpp"

using namespace pancake;

Window* Window::create() {
  return new GL3SDL3Window();
}