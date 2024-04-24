#include "input/input_action.hpp"

#include "core/input.hpp"

using namespace pancake;

InputAction::KeyboardMapping::KeyboardMapping(KBCode key) : key(key) {}

std::optional<bool> InputAction::KeyboardMapping::getValue(const Input& input) const {
  if (input.isKeyPressed(key)) {
    return true;
  }
  return std::nullopt;
}

InputAction::MouseMapping::MouseMapping(MouseCode button) : button(button) {}

std::optional<bool> InputAction::MouseMapping::getValue(const Input& input) const {
  if (input.isMousePressed(button)) {
    return true;
  }
  return std::nullopt;
}

InputAction::GamepadButtonMapping::GamepadButtonMapping(Gamepad::ButtonCode button, int slot)
    : button(button), slot(slot) {}

std::optional<bool> InputAction::GamepadButtonMapping::getValue(const Input& input) const {
  if (input.getGamepad(slot).isPressed(button)) {
    return true;
  }
  return std::nullopt;
}

InputAction::GamepadAxisMapping::GamepadAxisMapping(Gamepad::AxisCode axis,
                                                    float range_start,
                                                    float range_end,
                                                    int slot)
    : axis(axis), range_start(range_start), range_end(range_end), slot(slot) {}

std::optional<bool> InputAction::GamepadAxisMapping::getValue(const Input& input) const {
  float value = input.getGamepad(slot).getAxisF(axis);
  if ((range_start <= value) && (value <= range_end)) {
    return true;
  }
  return std::nullopt;
}

InputAction::InputAction() : _activated(false), _just_activated(false), _just_deactivated(false) {}

void InputAction::removeMapping(size_t index) {
  if (index < _mappings.size()) {
    auto it = _mappings.begin() + index;
    _mappings.erase(it);
  }
}

const InputAction::Mappings& InputAction::getMappings() {
  return _mappings;
}

void InputAction::update(const Input& input) {
  bool activate = false;
  for (const std::unique_ptr<Mapping>& mapping : _mappings) {
    if (mapping) {
      std::optional<bool> value = mapping->getValue(input);
      if (value.has_value()) {
        activate = value.value();
        break;
      }
    }
  }

  if (activate == _activated) {
    _just_activated = false;
    _just_deactivated = false;
  } else {
    _activated = activate;
    _just_activated = activate;
    _just_deactivated = !activate;
  }
}

bool InputAction::getActivated() const {
  return _activated;
}

bool InputAction::getJustActivated() const {
  return _just_activated;
}

bool InputAction::getJustDeactivated() const {
  return _just_deactivated;
}