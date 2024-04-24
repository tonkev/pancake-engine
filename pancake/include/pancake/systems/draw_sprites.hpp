#pragma once

#include "ecs/draw_system.hpp"

#include "pancake.hpp"
#include "util/guid.hpp"

namespace pancake {
class Mesh;
class Shader;
class DrawSprites : public DrawSystem {
 public:
  using DrawSystem::DrawSystem;
  virtual ~DrawSprites() = default;

  virtual std::string_view name() const override;
  virtual SystemId id() const override;

  virtual const SessionAccess& getSessionAccess() const override;
  virtual const ComponentAccess& getComponentAccess() const override;

 protected:
  virtual void _configure(Session& session) override;
  virtual void _run(const SessionWrapper& session, const WorldWrapper& world) const override;

 private:
  GUID _mesh = GUID::null;
  GUID _shader = GUID::null;
};
}  // namespace pancake