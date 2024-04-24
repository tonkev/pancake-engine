#pragma once

#include "util/matrix.hpp"

namespace pancake {
class Window {
 public:
  virtual ~Window() = default;

  virtual void newImGuiFrame() = 0;
  virtual void flip() = 0;

  virtual Vec2i size() const = 0;

  static Window* create();

 protected:
  Window() = default;
};
}  // namespace pancake