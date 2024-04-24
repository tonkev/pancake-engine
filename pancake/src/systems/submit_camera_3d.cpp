#include "systems/submit_camera_3d.hpp"

#include "components/3d.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<SubmitCamera3D> submit_camera_3d_adder{};

void SubmitCamera3D::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  for (const auto& [cam_transform, camera] :
       world.getComponents<const Transform3D, const Camera3D>()) {
    renderer.submitCamera(*cam_transform, *camera);
  }
}

std::string_view SubmitCamera3D::name() const {
  return "SubmitCamera3D";
}

SystemId SubmitCamera3D::id() const {
  return System::id<SubmitCamera3D>();
}

const SessionAccess& SubmitCamera3D::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& SubmitCamera3D::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Transform3D, const Camera3D>();
  return component_access;
}