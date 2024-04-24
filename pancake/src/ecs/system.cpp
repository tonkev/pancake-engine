#include "ecs/system.hpp"

#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/encompasser_access.hpp"
#include "ecs/message_access.hpp"
#include "ecs/world_wrapper.hpp"
#include "util/assert.hpp"
#include "util/fewi.hpp"

using namespace pancake;

System::System(Session& session) : _session(session) {}

void System::configure() {
  _configure(_session);
}

void System::run(SystemNodeId system_node) const {
  for (MessageId id : getMessageAccess().getSends()) {
    _session.messageBoard(id).clearFrom(system_node);
  }
  _run(SessionWrapper(_session, getSessionAccess(), getMessageAccess(), system_node));
}

void System::run(World& world, SystemNodeId system_node) const {
  for (MessageId id : getMessageAccess().getSends()) {
    _session.messageBoard(id, &world).clearFrom(system_node);
  }
  _run(SessionWrapper(_session, getSessionAccess(), getMessageAccess(), system_node, &world),
       WorldWrapper(world, getComponentAccess(), getEncompasserAccess()));
}

void System::_configure(Session& session) {}

void System::_run(const SessionWrapper& session) const {}
void System::_run(const SessionWrapper& session, const WorldWrapper& world) const {}

const System::SystemIdSet& System::precedingSystems() const {
  static const SystemIdSet preceding{};
  return preceding;
}

const System::SystemIdSet& System::succeedingSystems() const {
  static const SystemIdSet succeeding{};
  return succeeding;
}

const ComponentAccess& System::getComponentAccess() const {
  static const ComponentAccess component_access;
  return component_access;
}

const EncompasserAccess& System::getEncompasserAccess() const {
  static const EncompasserAccess encompasser_access;
  return encompasser_access;
}

const SessionAccess& System::getSessionAccess() const {
  static const SessionAccess session_access;
  return session_access;
}

const MessageAccess& System::getMessageAccess() const {
  static const MessageAccess message_access;
  return message_access;
}

bool System::intersects(const System& other) const {
  return getComponentAccess().intersects(other.getComponentAccess()) ||
         getEncompasserAccess().intersects(other.getEncompasserAccess()) ||
         getSessionAccess().intersects(other.getSessionAccess()) ||
         getMessageAccess().intersects(other.getMessageAccess()) ||
         precedingSystems().contains(other.id()) || succeedingSystems().contains(other.id()) ||
         other.precedingSystems().contains(id()) || other.succeedingSystems().contains(id());
}

SystemId System::nextId() {
  static SystemId next = 0;
  return next++;
}