#pragma once

#include "input/gamepad.hpp"

typedef uint32_t SDL_JoystickID;
struct SDL_Gamepad;

namespace pancake {
class SDL3Gamepad : public Gamepad {
 public:
  SDL3Gamepad(SDL_JoystickID id);
  virtual ~SDL3Gamepad();

  virtual void refresh() override;

  SDL_JoystickID id() const;

 private:
  SDL_JoystickID _id;
  SDL_Gamepad* _gamepad;
};
}  // namespace pancake