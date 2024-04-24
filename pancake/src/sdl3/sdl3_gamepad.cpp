#include "sdl3/sdl3_gamepad.hpp"

#include "util/fewi.hpp"

#include "SDL3/SDL.h"

using namespace pancake;

SDL3Gamepad::SDL3Gamepad(SDL_JoystickID id) : _id(id), _gamepad(SDL_OpenGamepad(id)) {
  if (nullptr == _gamepad) {
    FEWI::error(std::string("Failed to initialise SDL Gamepad ! : ") + SDL_GetError());
  }
}

SDL3Gamepad::~SDL3Gamepad() {
  SDL_CloseGamepad(_gamepad);
}

void SDL3Gamepad::refresh() {
  Gamepad::refresh();

  if (nullptr != _gamepad) {
    setButton(ButtonCode::DPAD_UP, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP));
    setButton(ButtonCode::DPAD_RIGHT,
              SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT));
    setButton(ButtonCode::DPAD_DOWN, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN));
    setButton(ButtonCode::DPAD_LEFT, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT));

    setButton(ButtonCode::FACE_UP, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_NORTH));
    setButton(ButtonCode::FACE_RIGHT, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_EAST));
    setButton(ButtonCode::FACE_DOWN, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_SOUTH));
    setButton(ButtonCode::FACE_LEFT, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_WEST));

    setButton(ButtonCode::SELECT, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_BACK));
    setButton(ButtonCode::START, SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_START));

    setButton(ButtonCode::LEFT_SHOULDER,
              SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER));
    setButton(
        ButtonCode::LEFT_TRIGGER,
        (SDL_GAMEPAD_AXIS_MAX / 2) < SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER));

    setButton(ButtonCode::RIGHT_SHOULDER,
              SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER));
    setButton(
        ButtonCode::RIGHT_TRIGGER,
        (SDL_GAMEPAD_AXIS_MAX / 2) < SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));

    setButton(ButtonCode::LEFT_ANALOG,
              SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK));
    setButton(ButtonCode::RIGHT_ANALOG,
              SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK));

    setAxis(AxisCode::LEFT_ANALOG_X, SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_LEFTX));
    setAxis(AxisCode::LEFT_ANALOG_Y, SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_LEFTY));

    setAxis(AxisCode::RIGHT_ANALOG_X, SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_RIGHTX));
    setAxis(AxisCode::RIGHT_ANALOG_Y, SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_RIGHTY));

    setAxis(AxisCode::LEFT_TRIGGER, SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
    setAxis(AxisCode::RIGHT_TRIGGER, SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));
  }
}

SDL_JoystickID SDL3Gamepad::id() const {
  return _id;
}
