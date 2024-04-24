#pragma once

#include "input/gamepad.hpp"
#include "input/kb_code.hpp"
#include "input/mouse_code.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace pancake {
class Input;
class InputAction {
 public:
  class Mapping {
   public:
    virtual std::optional<bool> getValue(const Input& input) const = 0;
  };

  class KeyboardMapping : public Mapping {
   public:
    KeyboardMapping(KBCode key);

    virtual std::optional<bool> getValue(const Input& input) const override;

    const KBCode key;
  };

  class MouseMapping : public Mapping {
   public:
    MouseMapping(MouseCode button);

    virtual std::optional<bool> getValue(const Input& input) const override;

    const MouseCode button;
  };

  class GamepadButtonMapping : public Mapping {
   public:
    GamepadButtonMapping(Gamepad::ButtonCode button, int slot);

    virtual std::optional<bool> getValue(const Input& input) const override;

    const Gamepad::ButtonCode button;
    const int slot;
  };

  class GamepadAxisMapping : public Mapping {
   public:
    GamepadAxisMapping(Gamepad::AxisCode axis, float range_start, float range_end, int slot);

    virtual std::optional<bool> getValue(const Input& input) const override;

    const Gamepad::AxisCode axis;
    const float range_start;
    const float range_end;
    const int slot;
  };

  using Mappings = std::vector<std::unique_ptr<Mapping>>;

  InputAction();

  template <typename T, typename... Args>
  void addMapping(Args&&... args) {
    _mappings.emplace_back(new T(std::forward<Args>(args)...));
  }

  void removeMapping(size_t index);

  const Mappings& getMappings();

  void update(const Input& input);

  bool getActivated() const;
  bool getJustActivated() const;
  bool getJustDeactivated() const;

 private:
  Mappings _mappings;

  bool _activated;
  bool _just_activated;
  bool _just_deactivated;
};
}  // namespace pancake