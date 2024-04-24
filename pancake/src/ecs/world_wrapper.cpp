#include "ecs/world_wrapper.hpp"

using namespace pancake;

WorldWrapper::WorldWrapper(World& world,
                           const ComponentAccess& comp_access,
                           const EncompasserAccess& enc_access)
    : _world(world), _comp_access(comp_access), _enc_access(enc_access) {}

EntityWrapper WorldWrapper::createEntity() const {
  ensure(ComponentMask::full() == _comp_access.getWrites());
  return _world.createEntity();
}

EntityWrapper WorldWrapper::getEntityWrapper(const Entity& ent) const {
  return {ent, _comp_access, _world};
}

const World& WorldWrapper::world() const {
  return _world;
}