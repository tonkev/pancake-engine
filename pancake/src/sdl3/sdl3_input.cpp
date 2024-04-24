#include "sdl3/sdl3_input.hpp"

#include "sdl3/sdl3_gamepad.hpp"
#include "util/fewi.hpp"

#include "SDL3/SDL.h"

using namespace pancake;

SDL3Input::SDL3Input() {
  if (0 > SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
    FEWI::fatal(std::string("Failed to initialise SDL Gamepad Subsystem! : ") + SDL_GetError());
  }
}

SDL3Input::~SDL3Input() {
  SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}

void SDL3Input::refresh() {
  SDL_UpdateGamepads();
  Input::refresh();
}

int SDL3Input::addGamepad(SDL_JoystickID id) {
  for (const auto& [slot, gamepad] : getGamepads()) {
    if (id == static_cast<SDL3Gamepad&>(*gamepad).id()) {
      return slot;
    }
  }
  return Input::addGamepad(new SDL3Gamepad(id));
}

void SDL3Input::removeGamepad(SDL_JoystickID id) {
  for (const auto& [slot, gamepad] : getGamepads()) {
    if (id == static_cast<SDL3Gamepad&>(*gamepad).id()) {
      Input::removeGamepad(slot);
      break;
    }
  }
}