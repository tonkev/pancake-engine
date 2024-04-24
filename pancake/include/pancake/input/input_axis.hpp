#pragma once

#include "input/gamepad.hpp"
#include "input/kb_code.hpp"
#include "input/mouse_code.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace pancake {
class Input;
class InputAxis {
 public:
  class Mapping {
   public:
    virtual std::optional<float> getValue(const Input& input) const = 0;
  };

  class KeyboardMapping : public Mapping {
   public:
    KeyboardMapping(KBCode key, float value);

    virtual std::optional<float> getValue(const Input& input) const override;

    const KBCode key;
    const float value;
  };

  class MouseMapping : public Mapping {
   public:
    MouseMapping(MouseCode button, float value);

    virtual std::optional<float> getValue(const Input& input) const override;

    const MouseCode button;
    const float value;
  };

  class GamepadButtonMapping : public Mapping {
   public:
    GamepadButtonMapping(Gamepad::ButtonCode button, float value, int slot);

    virtual std::optional<float> getValue(const Input& input) const override;

    const Gamepad::ButtonCode button;
    const float value;
    const int slot;
  };

  class GamepadAxisMapping : public Mapping {
   public:
    GamepadAxisMapping(Gamepad::AxisCode axis, float offset, float scale, float deadzone, int slot);

    virtual std::optional<float> getValue(const Input& input) const override;

    const Gamepad::AxisCode axis;
    const float offset;
    const float scale;
    const float deadzone;
    const int slot;
  };

  using Mappings = std::vector<std::unique_ptr<Mapping>>;

  InputAxis();

  template <typename T, typename... Args>
  void addMapping(Args&&... args) {
    _mappings.emplace_back(new T(std::forward<Args>(args)...));
  }

  void removeMapping(size_t index);

  const Mappings& getMappings();

  void update(const Input& input);

  float value() const;

 private:
  Mappings _mappings;

  float _value;
};
}  // namespace pancake