#include "systems/draw_mesh_instances.hpp"

#include "components/3d.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"
#include "graphics/shader.hpp"
#include "graphics/shader_input.hpp"
#include "systems/submit_lights.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<DrawMeshInstances> draw_mesh_instances_adder{};

void DrawMeshInstances::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  for (const auto& [base, transform, mesh, material] :
       world.getComponents<const Base, const Transform3D, const MeshInstance,
                           const MaterialInstance>()) {
    Mat4f model = transform->matrix();
    renderer.submit(mesh->camera_mask, material->material, {}, mesh->mesh, model, base->self);
  }
}

std::string_view DrawMeshInstances::name() const {
  return "DrawMeshInstances";
}

SystemId DrawMeshInstances::id() const {
  return System::id<DrawMeshInstances>();
}

const SessionAccess& DrawMeshInstances::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& DrawMeshInstances::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Base, const Transform3D, const MeshInstance,
                            const MaterialInstance>();
  return component_access;
}