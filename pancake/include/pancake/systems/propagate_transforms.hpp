#pragma once

#include "ecs/logic_system.hpp"
#include "util/matrix.hpp"

namespace pancake {
class PropagateTransforms : public LogicSystem {
 public:
  using LogicSystem::LogicSystem;

  virtual std::string_view name() const override;
  virtual SystemId id() const override;

  virtual const ComponentAccess& getComponentAccess() const override;

 protected:
  virtual void _run(const SessionWrapper& session, const WorldWrapper& world) const override;

 private:
  void recursor(const WorldWrapper& world, const Entity& parent, const Mat3f& parent_global_transform, const Mat3f& inv_parent_global_transform) const;
};
}  // namespace pancake