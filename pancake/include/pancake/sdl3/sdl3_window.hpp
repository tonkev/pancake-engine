#pragma once

#include "core/window.hpp"

#include <stdint.h>

struct SDL_Window;

namespace pancake {
class SDL3Window : public Window {
 public:
  virtual ~SDL3Window();

  virtual Vec2i size() const override;

 protected:
  SDL3Window(uint32_t sdl_window_flags);

  SDL_Window* _window;
};
}  // namespace pancake