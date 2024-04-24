#pragma once

#include "ecs/draw_system.hpp"
#include "pancake.hpp"

namespace pancake {
class SubmitFramebuffers : public DrawSystem {
 public:
  using DrawSystem::DrawSystem;
  virtual ~SubmitFramebuffers() = default;

  virtual std::string_view name() const override;
  virtual SystemId id() const override;

  virtual const SessionAccess& getSessionAccess() const override;
  virtual const ComponentAccess& getComponentAccess() const override;

 protected:
  virtual void _run(const SessionWrapper& session, const WorldWrapper& world) const override;
};
}  // namespace pancake