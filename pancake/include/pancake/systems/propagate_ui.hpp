#pragma once

#include "ecs/logic_system.hpp"

#include "components/ui.hpp"

namespace pancake {
class PropagateUI : public LogicSystem {
 public:
  using LogicSystem::LogicSystem;

  virtual std::string_view name() const override;
  virtual SystemId id() const override;

  virtual const SessionAccess& getSessionAccess() const override;
  virtual const ComponentAccess& getComponentAccess() const override;

 protected:
  virtual void _run(const SessionWrapper& session, const WorldWrapper& world) const override;

  static void recursor(int axis,
                       UIContainer& ui,
                       const Entity& ui_ent,
                       const UIContainer& parent_ui,
                       const WorldWrapper& world);
};
}  // namespace pancake