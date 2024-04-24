#include "core/session_wrapper.hpp"

#include "core/session.hpp"
#include "core/session_access.hpp"
#include "ecs/world_wrapper.hpp"

using namespace pancake;

SessionWrapper::SessionWrapper(Session& session,
                               const SessionAccess& session_access,
                               const MessageAccess& message_access,
                               SystemNodeId system_node,
                               World* world)
    : _session(session),
      _world(world),
      _session_access(session_access),
      _message_access(message_access),
      _system_node(system_node) {}

SessionWrapper::SessionWrapper(const SessionWrapper& wrapper)
    : _session(wrapper._session),
      _world(wrapper._world),
      _session_access(wrapper._session_access),
      _message_access(wrapper._message_access),
      _system_node(wrapper._system_node) {}

const Input& SessionWrapper::input() const {
  return _session.input();
}

Renderer& SessionWrapper::renderer() const {
  ensure(_session_access.hasRendererAccess());
  return _session.renderer();
}

Resources& SessionWrapper::resources() const {
  ensure(_session_access.hasResourcesAccess());
  return _session.resources();
}

float SessionWrapper::delta() const {
  return _session.delta();
}