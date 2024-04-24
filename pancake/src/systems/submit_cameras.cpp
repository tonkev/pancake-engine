#include "systems/submit_cameras.hpp"

#include "components/2d.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"
#include "util/fewi.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<SubmitCameras> submit_cameras_adder{};

void SubmitCameras::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  for (const auto& [cam_transform, camera] :
       world.getComponents<const Transform2D, const Camera2D>()) {
    renderer.submitCamera(*cam_transform, *camera);
  }
}

std::string_view SubmitCameras::name() const {
  return "SubmitCameras";
}

SystemId SubmitCameras::id() const {
  return System::id<SubmitCameras>();
}

const SessionAccess& SubmitCameras::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& SubmitCameras::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Transform2D, const Camera2D>();
  return component_access;
}