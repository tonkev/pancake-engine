#include "core/input.hpp"

#include "input/gamepad.hpp"
#include "input/input_action.hpp"
#include "input/input_axis.hpp"
#include "sdl3/sdl3_input.hpp"

using namespace pancake;

Input::~Input() {}

void Input::refresh() {
  _kb_just_pressed.clear();
  _kb_just_released.clear();
  _mouse_just_pressed.clear();
  _mouse_just_released.clear();
  _mouse_delta.x() = 0.f;
  _mouse_delta.y() = 0.f;
  _mouse_scroll_delta = 0.f;

  for (const auto& [_, gamepad] : _gamepads) {
    gamepad->refresh();
  }

  for (const auto& [_, action] : _actions) {
    action->update(*this);
  }

  for (const auto& [_, axis] : _axes) {
    axis->update(*this);
  }
}

void Input::keyJustPressed(KBCode key) {
  _kb_just_pressed.set(static_cast<int>(key));
  _kb_pressed.set(static_cast<int>(key));
}

void Input::keyJustReleased(KBCode key) {
  _kb_just_released.set(static_cast<int>(key));
  _kb_pressed.unset(static_cast<int>(key));
}

void Input::mouseJustPressed(MouseCode button) {
  _mouse_just_pressed.set(static_cast<int>(button));
  _mouse_pressed.set(static_cast<int>(button));
}

void Input::mouseJustReleased(MouseCode button) {
  _mouse_just_released.set(static_cast<int>(button));
  _mouse_pressed.unset(static_cast<int>(button));
}

void Input::mouseMoved(float x, float y, float dx, float dy) {
  _mouse_position.x() = x;
  _mouse_position.y() = y;
  _mouse_delta.x() += dx;
  _mouse_delta.y() += dy;
}

void Input::mouseScrolled(float d) {
  _mouse_scroll_delta += d;
}

int Input::addGamepad(Gamepad* gamepad) {
  int slot = -1;
  while ((_gamepads.contains(++slot)) && (slot < 64))
    ;
  if (slot < 64) {
    _gamepads.emplace(slot, gamepad);
  } else {
    // ensure we don't get stuck in a loop if an
    // ungodly amount of gamepads are connected
    slot = -1;
    delete gamepad;
  }
  return slot;
}

void Input::removeGamepad(int slot) {
  _gamepads.erase(slot);
}

bool Input::isKeyJustPressed(KBCode key) const {
  return _kb_just_pressed[static_cast<int>(key)];
}

bool Input::isKeyJustReleased(KBCode key) const {
  return _kb_just_released[static_cast<int>(key)];
}

bool Input::isKeyPressed(KBCode key) const {
  return _kb_pressed[static_cast<int>(key)];
}

bool Input::isMouseJustPressed(MouseCode button) const {
  return _mouse_just_pressed[static_cast<int>(button)];
}

bool Input::isMouseJustReleased(MouseCode button) const {
  return _mouse_just_released[static_cast<int>(button)];
}

bool Input::isMousePressed(MouseCode button) const {
  return _mouse_pressed[static_cast<int>(button)];
}

const Vec2f& Input::getMousePosition() const {
  return _mouse_position;
}

const Vec2f& Input::getMouseDelta() const {
  return _mouse_delta;
}

float Input::getMouseScrollDelta() const {
  return _mouse_scroll_delta;
}

const Gamepad& Input::getGamepad(int slot) const {
  static const Gamepad fallback;
  if (_gamepads.contains(slot)) {
    return *_gamepads.at(slot);
  }
  return fallback;
}

InputAction& Input::getOrCreateAction(std::string_view name) {
  if (_actions.contains(name)) {
    return *_actions.find(name)->second.get();
  }
  return *_actions.emplace(name, new InputAction()).first->second.get();
}

const InputAction& Input::getAction(std::string_view name) const {
  static const InputAction fallback;
  if (_actions.contains(name)) {
    return *_actions.find(name)->second.get();
  }
  return fallback;
}

InputAxis& Input::getOrCreateAxis(std::string_view name) {
  if (_axes.contains(name)) {
    return *_axes.find(name)->second.get();
  }
  return *_axes.emplace(name, new InputAxis()).first->second.get();
}

const InputAxis& Input::getAxis(std::string_view name) const {
  static const InputAxis fallback;
  if (_axes.contains(name)) {
    return *_axes.find(name)->second.get();
  }
  return fallback;
}

const Input::GamepadMap& Input::getGamepads() {
  return _gamepads;
}

Input* Input::create() {
  return new SDL3Input();
}