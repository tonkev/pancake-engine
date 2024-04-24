#include "input/gamepad.hpp"

#include <algorithm>
#include <limits>

using namespace pancake;

Gamepad::Gamepad() {
  for (int16_t& axis : _axis_values) {
    axis = 0;
  }
}

void Gamepad::refresh() {
  _just_pressed.clear();
  _just_released.clear();
}

void Gamepad::justPressed(ButtonCode button) {
  _just_pressed.set(static_cast<int>(button));
  _pressed.set(static_cast<int>(button));
}

void Gamepad::justReleased(ButtonCode button) {
  _just_released.set(static_cast<int>(button));
  _pressed.unset(static_cast<int>(button));
}

void Gamepad::setButton(ButtonCode button, bool pressed) {
  if (isPressed(button)) {
    if (!pressed) {
      justReleased(button);
    }
  } else if (pressed) {
    justPressed(button);
  }
}

void Gamepad::setAxis(AxisCode axis, int16_t value) {
  _axis_values[static_cast<int>(axis)] = value;
}

bool Gamepad::isJustPressed(ButtonCode button) const {
  return _just_pressed.get(static_cast<int>(button));
}

bool Gamepad::isJustReleased(ButtonCode button) const {
  return _just_released.get(static_cast<int>(button));
}

bool Gamepad::isPressed(ButtonCode button) const {
  return _pressed.get(static_cast<int>(button));
}

int16_t Gamepad::getAxis(AxisCode axis) const {
  return _axis_values[static_cast<int>(axis)];
}

float Gamepad::getAxisF(AxisCode axis) const {
  return std::clamp(_axis_values[static_cast<int>(axis)] /
                        static_cast<float>(std::numeric_limits<int16_t>::max()),
                    -1.0f, 1.0f);
}

Vec2f Gamepad::getLeftAnalogAxes() const {
  return Vec2f(getAxisF(AxisCode::LEFT_ANALOG_X), getAxisF(AxisCode::LEFT_ANALOG_Y));
}

Vec2f Gamepad::getRightAnalogAxes() const {
  return Vec2f(getAxisF(AxisCode::RIGHT_ANALOG_X), getAxisF(AxisCode::RIGHT_ANALOG_Y));
}