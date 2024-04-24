#include "core/event_handler.hpp"

#include "sdl3/sdl3_event_handler.hpp"

using namespace pancake;

EventHandler* EventHandler::create() {
  return new SDL3EventHandler();
}
