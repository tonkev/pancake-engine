#include "core/session.hpp"

#include "core/event_handler.hpp"
#include "core/input.hpp"
#include "core/renderer.hpp"
#include "core/window.hpp"
#include "ecs/default_components.hpp"
#include "ecs/draw_system.hpp"
#include "ecs/logic_system.hpp"
#include "ecs/messages.hpp"
#include "util/fewi.hpp"

#if defined(PANCAKE_ENABLE_IMGUI)
#include "imgui/imgui.h"
#endif

#include <algorithm>
#include <chrono>
#include <ranges>
#include <thread>

namespace chrono = std::chrono;

using namespace pancake;

Session::Session(SessionConfig&& config)
    : _config(std::move(config)),
      _event_handler(EventHandler::create()),
      _input(Input::create()),
      _global_messages(true),
      _window(Window::create()),
      _renderer(Renderer::create(_resources)),
      _time(0.f) {}

Session::~Session() {
  delete _renderer;
  delete _window;
  delete _input;
  delete _event_handler;
}

void Session::addWorld(const Ptr<World>& world) {
  _worlds.insert(world);
}

void Session::removeWorld(const Ptr<World>& world) {
  _worlds.erase(world);
}

const chrono::microseconds target_frame_duration(16670);
const float target_delta = target_frame_duration.count() * 0.000001f;

void Session::run() {
  const FEWI& fewi = FEWI::get();

  registerComponents();
  configure();

  _renderer->setScreenSize(_window->size());
  _renderer->init();

  _logic_system_graph.build(*LogicSystem::createLogicSystems(*this), _worlds);
  _draw_system_graph.build(*DrawSystem::createDrawSystems(*this), _worlds);
  for (const SystemGraph::Node& node : _logic_system_graph.nodes()) {
    node.system()->configure();
  }
  for (const SystemGraph::Node& node : _draw_system_graph.nodes()) {
    node.system()->configure();
  }

  if (const auto* rule = _config.getRule<LogSystemGraphsRule>();
      (rule != nullptr) && rule->value()) {
    {
      FEWI::MessageStream logic_msg = FEWI::info();
      _logic_system_graph.visualise(logic_msg);
    }

    {
      FEWI::MessageStream draw_msg = FEWI::info();
      _draw_system_graph.visualise(draw_msg);
    }
  }

  chrono::time_point prev_timestamp = chrono::high_resolution_clock::now();
  chrono::nanoseconds accumulator_dur(0);

  bool quitting = (FEWI::Severity::Fatal == fewi.max_severity());
  while (!quitting) {
    chrono::time_point timestamp = chrono::high_resolution_clock::now();
    chrono::nanoseconds frame_dur(timestamp - prev_timestamp);

    if (target_frame_duration > frame_dur) {
      std::this_thread::sleep_for(target_frame_duration - frame_dur);
      timestamp = chrono::high_resolution_clock::now();
      frame_dur = timestamp - prev_timestamp;
    }
    prev_timestamp = timestamp;

    accumulator_dur += frame_dur;
    while (accumulator_dur >= target_frame_duration) {
#if defined(PANCAKE_ENABLE_IMGUI)
      _window->newImGuiFrame();
#endif
      _input->refresh();
      quitting =
          _event_handler->handleEvents(*this) || (FEWI::Severity::Fatal == fewi.max_severity());
      _dispatcher.execute(_logic_system_graph);

      accumulator_dur -= target_frame_duration;
      _time += target_delta;
    }

    _dispatcher.execute(_draw_system_graph);
    _renderer->preRender(*this, _resources);
    _renderer->render();
    _window->flip();
  }
}

const SessionConfig& Session::config() {
  return _config;
}

Input& Session::input() {
  return *_input;
}

Window& Session::window() {
  return *_window;
}

const Session::Worlds& Session::worlds() {
  return _worlds;
}

MessageBoards& Session::globalMessages() {
  return _global_messages;
}

Renderer& Session::renderer() {
  return *_renderer;
}

Resources& Session::resources() {
  return _resources;
}

float Session::delta() const {
  return target_delta;
}

float Session::time() const {
  return _time;
}

MessageBoard& Session::messageBoard(MessageId message_id, World* world) {
  if (Messages::isGlobal(message_id)) {
    _global_messages.getMessageBoard(message_id);
  }

  ensure(nullptr != world);
  return world->getLocalMessages().getMessageBoard(message_id);
}

void Session::registerComponents() const {
  Components::get().add(default_components::get());
}