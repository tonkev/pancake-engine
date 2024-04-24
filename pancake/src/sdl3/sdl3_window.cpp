#include "sdl3/sdl3_window.hpp"

#include "util/fewi.hpp"

#include "SDL3/SDL.h"

using namespace pancake;

SDL3Window::SDL3Window(uint32_t sdl_window_flags) {
  if (0 <= SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    _window = SDL_CreateWindow("pancake", 1024, 800, SDL_WINDOW_RESIZABLE | sdl_window_flags);

    if (!_window) {
      FEWI::fatal(std::string("Failed to create SDL Window! : ") + SDL_GetError());
    }
  } else {
    FEWI::fatal(std::string("Failed to initialise SDL Video Subsystem! : ") + SDL_GetError());
  }
}

SDL3Window::~SDL3Window() {
  SDL_DestroyWindow(_window);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

Vec2i SDL3Window::size() const {
  Vec2i out;
  SDL_GetWindowSize(_window, &out.x(), &out.y());
  return out;
}