#pragma once

#include "core/dispatcher.hpp"
#include "core/session_config.hpp"
#include "ecs/message_boards.hpp"
#include "ecs/system_graph.hpp"
#include "ecs/world.hpp"
#include "resources/resources.hpp"

#include <set>

namespace pancake {
class EventHandler;
class Input;
class Renderer;
class Window;
class Session {
 public:
  using Worlds = std::set<Ptr<World>>;

  Session(SessionConfig&& config);
  virtual ~Session();

  void addWorld(const Ptr<World>& world);
  void removeWorld(const Ptr<World>& world);

  void run();

  const SessionConfig& config();
  Input& input();
  Window& window();
  const Worlds& worlds();
  MessageBoards& globalMessages();
  Renderer& renderer();
  Resources& resources();

  float delta() const;
  float time() const;

  MessageBoard& messageBoard(MessageId message_id, World* world = nullptr);

  static Session* create(SessionConfig&& config);

 protected:
  virtual void registerComponents() const;
  virtual void configure() = 0;

 private:
  SessionConfig&& _config;
  Resources _resources;

  EventHandler* _event_handler;
  Input* _input;
  Window* _window;
  Renderer* _renderer;

  Worlds _worlds;
  MessageBoards _global_messages;

  Dispatcher _dispatcher;

  SystemGraph _logic_system_graph;
  SystemGraph _draw_system_graph;

  float _time;
};
}  // namespace pancake