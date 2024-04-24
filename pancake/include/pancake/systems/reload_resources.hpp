#pragma once

#include "ecs/logic_system.hpp"
#include "pancake.hpp"

namespace pancake {
class ReloadResources : public LogicSystem {
 public:
  using LogicSystem::LogicSystem;

  virtual std::string_view name() const override;
  virtual SystemId id() const override;

  virtual const SessionAccess& getSessionAccess() const override;

 protected:
  virtual void _run(const SessionWrapper& session) const override;
};
}  // namespace pancake