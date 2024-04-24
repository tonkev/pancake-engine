#include "systems/submit_lights.hpp"

#include "components/3d.hpp"
#include "components/graphics.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"
#include "graphics/light_info.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<SubmitLights> submit_lights_adder{};

void SubmitLights::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  for (const auto& [transform, light] :
       world.getComponents<const Transform3D, const PointLight>()) {
    renderer.submitLight(LightInfo(*transform, *light));
  }
}

std::string_view SubmitLights::name() const {
  return "SubmitLights";
}

SystemId SubmitLights::id() const {
  return System::id<SubmitLights>();
}

const SessionAccess& SubmitLights::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& SubmitLights::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Transform3D, const PointLight>();
  return component_access;
}