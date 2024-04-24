#include "sdl3/sdl3_event_handler.hpp"

#include "core/renderer.hpp"
#include "core/session.hpp"
#include "sdl3/sdl3_input.hpp"
#include "util/fewi.hpp"

#include "SDL3/SDL.h"

#if defined(PANCAKE_ENABLE_IMGUI)
#include "imgui/imgui.h"

#include "imgui/backends/imgui_impl_sdl3.h"
#endif

using namespace pancake;

SDL3EventHandler::SDL3EventHandler() {
  if (0 > SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
    FEWI::fatal(std::string("Failed to initialise SDL Event Subsystem! : ") + SDL_GetError());
  }
}

SDL3EventHandler::~SDL3EventHandler() {
  SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
}

bool SDL3EventHandler::handleEvents(Session& session) {
  static const auto to_kb_code = [](SDL_Keycode key) {
    if (128 <= key) {
      key -= 1073741754;
    }
    if (static_cast<int>(KBCode::RightGui) < key) {
      key = 1;
    }
    return static_cast<KBCode>(key);
  };

  bool ignoreMouse = false;
  bool ignoreKeyboard = false;
#if defined(PANCAKE_ENABLE_IMGUI)
  ImGuiIO& io = ImGui::GetIO();
  ignoreMouse = io.WantCaptureMouse;
  ignoreKeyboard = io.WantCaptureKeyboard;
#endif

  bool quitting = false;
  SDL3Input& input = static_cast<SDL3Input&>(session.input());
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        quitting = true;
        break;
      case SDL_EVENT_GAMEPAD_ADDED:
        input.addGamepad(event.cdevice.which);
        break;
      case SDL_EVENT_GAMEPAD_REMOVED:
        input.removeGamepad(event.cdevice.which);
        break;
      case SDL_EVENT_MOUSE_MOTION:
        input.mouseMoved(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
        break;
      case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        session.renderer().setScreenSize(Vec2i(event.window.data1, event.window.data2));
        break;
      default:
        break;
    }
    if (!ignoreMouse) {
      switch (event.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
          input.mouseJustPressed(static_cast<MouseCode>(event.button.button));
          break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
          input.mouseJustReleased(static_cast<MouseCode>(event.button.button));
          break;
        case SDL_EVENT_MOUSE_WHEEL:
          input.mouseScrolled(event.wheel.y);
          break;
        default:
          break;
      }
    }
    if (!ignoreKeyboard) {
      switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
          input.keyJustPressed(to_kb_code(event.key.keysym.sym));
          break;
        case SDL_EVENT_KEY_UP:
          input.keyJustReleased(to_kb_code(event.key.keysym.sym));
          break;
        default:
          break;
      }
    }
#if defined(PANCAKE_ENABLE_IMGUI)
    ImGui_ImplSDL3_ProcessEvent(&event);
#endif
  }

  return quitting;
}