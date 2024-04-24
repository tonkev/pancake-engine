#pragma once

#include "input/input_action.hpp"
#include "input/input_axis.hpp"
#include "input/kb_code.hpp"
#include "input/mouse_code.hpp"
#include "util/bitmask.hpp"
#include "util/matrix.hpp"

#include <map>

namespace pancake {
class Gamepad;
class Input {
 public:
  virtual ~Input();

  virtual void refresh();

  void keyJustPressed(KBCode key);
  void keyJustReleased(KBCode key);

  void mouseJustPressed(MouseCode button);
  void mouseJustReleased(MouseCode button);
  void mouseMoved(float x, float y, float dx, float dy);
  void mouseScrolled(float d);

  bool isKeyJustPressed(KBCode key) const;
  bool isKeyJustReleased(KBCode key) const;
  bool isKeyPressed(KBCode key) const;

  bool isMouseJustPressed(MouseCode button) const;
  bool isMouseJustReleased(MouseCode button) const;
  bool isMousePressed(MouseCode button) const;

  const Vec2f& getMousePosition() const;
  const Vec2f& getMouseDelta() const;
  float getMouseScrollDelta() const;

  const Gamepad& getGamepad(int slot) const;

  InputAction& getOrCreateAction(std::string_view name);
  const InputAction& getAction(std::string_view name) const;

  InputAxis& getOrCreateAxis(std::string_view name);
  const InputAxis& getAxis(std::string_view name) const;

  static Input* create();

 protected:
  Input() = default;

  using GamepadMap = std::map<int, std::unique_ptr<Gamepad>>;

  int addGamepad(Gamepad* gamepad);
  void removeGamepad(int slot);

  const GamepadMap& getGamepads();

 private:
  using KeyboardMask = Bitmask<5>;
  using MouseMask = Bitmask<1>;

  KeyboardMask _kb_just_pressed;
  KeyboardMask _kb_just_released;
  KeyboardMask _kb_pressed;

  MouseMask _mouse_just_pressed;
  MouseMask _mouse_just_released;
  MouseMask _mouse_pressed;
  Vec2f _mouse_position;
  Vec2f _mouse_delta;
  float _mouse_scroll_delta;

  GamepadMap _gamepads;

  std::map<std::string, std::unique_ptr<InputAction>, std::less<>> _actions;
  std::map<std::string, std::unique_ptr<InputAxis>, std::less<>> _axes;
};
}  // namespace pancake