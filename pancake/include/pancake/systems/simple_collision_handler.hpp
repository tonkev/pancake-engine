#pragma once

#include "ecs/logic_system.hpp"

namespace pancake {
class SimpleCollisionHandler : public LogicSystem {
  using LogicSystem::LogicSystem;

  virtual std::string_view name() const override;
  virtual SystemId id() const override;

  virtual const ComponentAccess& getComponentAccess() const override;
  virtual const EncompasserAccess& getEncompasserAccess() const override;

 protected:
  virtual void _run(const SessionWrapper& session, const WorldWrapper& world) const override;
};
}  // namespace pancake