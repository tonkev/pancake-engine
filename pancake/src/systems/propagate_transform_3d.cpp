#include "systems/propagate_transform_3d.hpp"

#include "components/3d.hpp"
#include "components/core.hpp"
#include "ecs/world_wrapper.hpp"
#include "util/matrix.hpp"

using namespace pancake;

LogicSystem::StaticAdder<PropagateTransform3D> propagate_transform_3d_adder{};

void recursor(const WorldWrapper& world,
              const Entity& parent,
              const Mat4f& parent_global_transform,
              const Mat4f& inv_parent_global_transform) {
  for (const auto& [base, transform] :
       world.getChildrenComponents<const Base, Transform3D>(parent)) {
    transform->setParentGlobalMatrices(parent_global_transform, inv_parent_global_transform);
    recursor(world, base->self, transform->matrix(), transform->inverseMatrix());
  }
}

void PropagateTransform3D::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  for (const auto& [root_base, root_transform] : world.getComponents<const Base, Transform3D>()) {
    if ((Entity::null == root_base->parent) ||
        (!world.hasComponent<Transform3D>(root_base->parent))) {
      const Mat4f& identity = Mat4f::identity();

      root_transform->_parent_global_transform = identity;
      root_transform->_inv_parent_global_transform = identity;

      root_transform->_translation = root_transform->_local_translation;
      root_transform->_scale = root_transform->_local_scale;
      root_transform->_rotation = root_transform->_local_rotation;
      root_transform->_state = Transform3D::State::Clean;

      recursor(world, root_base->self, root_transform->matrix(), root_transform->inverseMatrix());
    }
  }
}

std::string_view PropagateTransform3D::name() const {
  return "PropagateTransform3D";
}

SystemId PropagateTransform3D::id() const {
  return System::id<PropagateTransform3D>();
}

const ComponentAccess& PropagateTransform3D::getComponentAccess() const {
  static const ComponentAccess component_access = Components::getAccess<const Base, Transform3D>();
  return component_access;
}
