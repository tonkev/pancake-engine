#pragma once

#include "core/session.hpp"
#include "ecs/message_board.hpp"
#include "ecs/messages.hpp"
#include "util/type_desc_library.hpp"

#include <functional>

namespace pancake {
class Input;
class Renderer;
class Resources;
class SessionAccess;

class SessionWrapper {
 public:
  SessionWrapper(Session& session,
                 const SessionAccess& session_access,
                 const MessageAccess& message_access,
                 SystemNodeId system_node,
                 World* world = nullptr);
  SessionWrapper(const SessionWrapper& wrapper);

  const Input& input() const;
  Renderer& renderer() const;
  Resources& resources() const;

  float delta() const;

  template <typename T>
  std::span<const MessageBoard::MessagePair<T>> getMessages() const {
    const TypeDesc& desc = TypeDescLibrary::get<T>();
    ensure(Messages::isMessage(desc));
    ensure(_message_access.getReceives().get(Messages::getId(desc)));

    return _session.messageBoard(Messages::getId(desc), _world).getMessages<T>();
  }

  template <typename T>
  void sendMessage(const T& message) const {
    const TypeDesc& desc = TypeDescLibrary::get<T>();
    ensure(Messages::isMessage(desc));
    ensure(_message_access.getSends().get(Messages::getId(desc)));

    _session.messageBoard(Messages::getId(desc), _world).add(message, _system_node);
  }

 private:
  Session& _session;
  World* _world;
  const SessionAccess& _session_access;
  const MessageAccess& _message_access;
  SystemNodeId _system_node;
};
}  // namespace pancake