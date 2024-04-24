#pragma once

#include "core/event_handler.hpp"

namespace pancake {
class SDL3EventHandler : public EventHandler {
 public:
  SDL3EventHandler();
  virtual ~SDL3EventHandler();

  virtual bool handleEvents(Session& session) override;
};
}  // namespace pancake