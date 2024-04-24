#pragma once

#include "core/input.hpp"

typedef uint32_t SDL_JoystickID;

namespace pancake {
class SDL3Input : public Input {
 public:
  SDL3Input();
  ~SDL3Input();

  virtual void refresh() override;

  int addGamepad(SDL_JoystickID id);
  void removeGamepad(SDL_JoystickID id);
};
}  // namespace pancake
