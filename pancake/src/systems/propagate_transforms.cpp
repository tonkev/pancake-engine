#include "systems/propagate_transforms.hpp"

#include "components/2d.hpp"
#include "components/core.hpp"
#include "ecs/world_wrapper.hpp"

using namespace pancake;

LogicSystem::StaticAdder<PropagateTransforms> propagate_transforms_adder{};

void PropagateTransforms::recursor(const WorldWrapper& world,
                                   const Entity& parent,
                                   const Mat3f& parent_global_transform,
                                   const Mat3f& inv_parent_global_transform) const {
  for (const auto& [base, transform] :
       world.getChildrenComponents<const Base, Transform2D>(parent)) {
    transform->setParentGlobalMatrices(parent_global_transform, inv_parent_global_transform);
    recursor(world, base->self, transform->matrix(), transform->inverseMatrix());
  }
}

void PropagateTransforms::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  for (const auto& [root_base, root_transform] : world.getComponents<const Base, Transform2D>()) {
    if ((Entity::null == root_base->parent) ||
        (!world.hasComponent<Transform2D>(root_base->parent))) {
      const Mat3f& identity = Mat3f::identity();

      root_transform->_parent_global_transform = identity;
      root_transform->_inv_parent_global_transform = identity;

      root_transform->_translation = root_transform->_local_translation;
      root_transform->_scale = root_transform->_local_scale;
      root_transform->_rotation = root_transform->_local_rotation;
      root_transform->_state = Transform2D::State::Clean;

      recursor(world, root_base->self, root_transform->matrix(), root_transform->inverseMatrix());
    }
  }
}

std::string_view PropagateTransforms::name() const {
  return "PropagateTransforms";
}

SystemId PropagateTransforms::id() const {
  return System::id<PropagateTransforms>();
}

const ComponentAccess& PropagateTransforms::getComponentAccess() const {
  static const ComponentAccess component_access = Components::getAccess<const Base, Transform2D>();
  return component_access;
}
