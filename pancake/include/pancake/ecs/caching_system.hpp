#pragma once

#include "core/session.hpp"
#include "ecs/components.hpp"
#include "ecs/system.hpp"
#include "ecs/world_wrapper.hpp"

#include <unordered_map>

namespace pancake {
class World;

template <typename Cache, class Parent>
class CachingSystem : public Parent {
 public:
  virtual ~CachingSystem() = default;

 protected:
  using Parent::Parent;

  virtual void _run(const SessionWrapper& session, Cache& cache) const {}
  virtual void _run(const SessionWrapper& session, const WorldWrapper& world, Cache& cache) const {}

  virtual void _configure(Session& session) override final {
    static_assert(std::is_base_of_v<System, Parent>);

    if (ComponentAccess::empty == this->getComponentAccess()) {
      _caches[nullptr];
    } else {
      for (const Ptr<World>& world : session.worlds()) {
        _caches[world.get()];
      }
    }
  }

  virtual void _run(const SessionWrapper& session) const override final {
    _run(session, _caches.at(nullptr));
  }

  virtual void _run(const SessionWrapper& session, const WorldWrapper& world) const override final {
    _run(session, world, _caches.at(&world.world()));
  }

 private:
  mutable std::unordered_map<const World*, Cache> _caches;
};
}  // namespace pancake