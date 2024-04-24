#pragma once

#include "util/bitmask.hpp"
#include "util/matrix.hpp"

#include <cstdint>

namespace pancake {
class Gamepad {
 public:
  enum class ButtonCode : int {
    DPAD_UP,
    DPAD_RIGHT,
    DPAD_DOWN,
    DPAD_LEFT,
    FACE_UP,
    FACE_RIGHT,
    FACE_DOWN,
    FACE_LEFT,
    SELECT,
    START,
    LEFT_SHOULDER,
    LEFT_TRIGGER,
    RIGHT_SHOULDER,
    RIGHT_TRIGGER,
    LEFT_ANALOG,
    RIGHT_ANALOG
  };

  enum class AxisCode : int {
    LEFT_ANALOG_X,
    LEFT_ANALOG_Y,
    RIGHT_ANALOG_X,
    RIGHT_ANALOG_Y,
    LEFT_TRIGGER,
    RIGHT_TRIGGER
  };

  Gamepad();
  virtual ~Gamepad() = default;

  virtual void refresh();

  void justPressed(ButtonCode button);
  void justReleased(ButtonCode button);

  void setButton(ButtonCode button, bool pressed);
  void setAxis(AxisCode axis, int16_t value);

  bool isJustPressed(ButtonCode button) const;
  bool isJustReleased(ButtonCode button) const;
  bool isPressed(ButtonCode button) const;

  int16_t getAxis(AxisCode axis) const;
  float getAxisF(AxisCode axis) const;

  Vec2f getLeftAnalogAxes() const;
  Vec2f getRightAnalogAxes() const;

 private:
  using ButtonMask = Bitmask<1>;

  ButtonMask _just_pressed;
  ButtonMask _just_released;
  ButtonMask _pressed;

  int16_t _axis_values[6];
};
}  // namespace pancake