#pragma once

#include "ecs/common.hpp"
#include "pancake.hpp"
#include "util/containers.hpp"

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace pancake {
using SystemId = uint32_t;

class ComponentAccess;
class EncompasserAccess;
class SessionAccess;
class SessionWrapper;
class Session;
class MessageAccess;
class WorldWrapper;
class World;

class System {
 public:
  using SystemIdSet = std::unordered_set<SystemId>;

  virtual ~System() = default;

  void configure();
  void run(SystemNodeId system_node) const;
  void run(World& world, SystemNodeId system_node) const;

  virtual std::string_view name() const = 0;
  virtual SystemId id() const = 0;

  virtual const SystemIdSet& precedingSystems() const;
  virtual const SystemIdSet& succeedingSystems() const;

  virtual const ComponentAccess& getComponentAccess() const;
  virtual const EncompasserAccess& getEncompasserAccess() const;
  virtual const SessionAccess& getSessionAccess() const;
  virtual const MessageAccess& getMessageAccess() const;

  bool intersects(const System& other) const;

  template <typename T>
  static SystemId id() {
    static_assert(std::is_base_of_v<System, T>);
    static const SystemId _id = nextId();
    return _id;
  }

 protected:
  System(Session& session);

  virtual void _configure(Session& session);
  virtual void _run(const SessionWrapper& session) const;
  virtual void _run(const SessionWrapper& session, const WorldWrapper& world) const;

 private:
  static SystemId nextId();

  Session& _session;
};

using Systems = std::vector<Ptr<System>>;
}  // namespace pancake