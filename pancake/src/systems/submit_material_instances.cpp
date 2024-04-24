#include "systems/submit_material_instances.hpp"

#include "components/graphics.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<SubmitMaterialInstances> submit_material_instances_adder{};

void SubmitMaterialInstances::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  Resources& resources = session.resources();
  for (const auto& [material_inst] : world.getComponents<const MaterialInstance>()) {
    renderer.submitMaterial(material_inst->material, resources);
  }
}

std::string_view SubmitMaterialInstances::name() const {
  return "SubmitMaterialInstances";
}

SystemId SubmitMaterialInstances::id() const {
  return System::id<SubmitMaterialInstances>();
}

const SessionAccess& SubmitMaterialInstances::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer().addResources();
  return session_access;
}

const ComponentAccess& SubmitMaterialInstances::getComponentAccess() const {
  static const ComponentAccess component_access = Components::getAccess<const MaterialInstance>();
  return component_access;
}