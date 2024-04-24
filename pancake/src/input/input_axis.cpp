#include "input/input_axis.hpp"

#include "core/input.hpp"

using namespace pancake;

InputAxis::KeyboardMapping::KeyboardMapping(KBCode key, float value) : key(key), value(value) {}

std::optional<float> InputAxis::KeyboardMapping::getValue(const Input& input) const {
  if (input.isKeyPressed(key)) {
    return value;
  }
  return std::nullopt;
}

InputAxis::MouseMapping::MouseMapping(MouseCode button, float value)
    : button(button), value(value) {}

std::optional<float> InputAxis::MouseMapping::getValue(const Input& input) const {
  if (input.isMousePressed(button)) {
    return value;
  }
  return std::nullopt;
}

InputAxis::GamepadButtonMapping::GamepadButtonMapping(Gamepad::ButtonCode button,
                                                      float value,
                                                      int slot)
    : button(button), value(value), slot(slot) {}

std::optional<float> InputAxis::GamepadButtonMapping::getValue(const Input& input) const {
  if (input.getGamepad(slot).isPressed(button)) {
    return value;
  }
  return std::nullopt;
}

InputAxis::GamepadAxisMapping::GamepadAxisMapping(Gamepad::AxisCode axis,
                                                  float offset,
                                                  float scale,
                                                  float deadzone,
                                                  int slot)
    : axis(axis), offset(offset), scale(scale), deadzone(deadzone), slot(slot) {}

std::optional<float> InputAxis::GamepadAxisMapping::getValue(const Input& input) const {
  float value = input.getGamepad(slot).getAxisF(axis);
  if (deadzone < std::abs(value)) {
    return (value * scale) + offset;
  }
  return std::nullopt;
}

InputAxis::InputAxis() : _value(0.f) {}

void InputAxis::removeMapping(size_t index) {
  if (index < _mappings.size()) {
    auto it = _mappings.begin() + index;
    _mappings.erase(it);
  }
}

const InputAxis::Mappings& InputAxis::getMappings() {
  return _mappings;
}

void InputAxis::update(const Input& input) {
  _value = 0.f;
  int total = 0;
  for (const std::unique_ptr<Mapping>& mapping : _mappings) {
    if (mapping) {
      std::optional<float> value = mapping->getValue(input);
      if (value.has_value()) {
        _value += value.value();
        ++total;
      }
    }
  }
  if (1 < total) {
    _value /= total;
  }
}

float InputAxis::value() const {
  return _value;
}