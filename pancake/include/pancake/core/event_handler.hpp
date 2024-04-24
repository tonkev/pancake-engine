#pragma once

namespace pancake {
class Session;
class EventHandler {
 public:
  virtual ~EventHandler() = default;

  virtual bool handleEvents(Session& session) = 0;

  static EventHandler* create();

 protected:
  EventHandler() = default;
};
}  // namespace pancake