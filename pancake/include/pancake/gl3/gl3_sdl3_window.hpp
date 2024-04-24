#pragma once

#include "sdl3/sdl3_window.hpp"

namespace pancake {
class GL3SDL3Window : public SDL3Window {
 public:
  GL3SDL3Window();
  virtual ~GL3SDL3Window();

  virtual void newImGuiFrame() override;
  virtual void flip() override;

 private:
  void* _gl_context;
};
}  // namespace pancake