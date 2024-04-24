#include "encompassers/physics_state.hpp"

#include "ecs/encompassers.hpp"

using namespace pancake;

static Encompassers::StaticAdder<PhysicsState> physics_state_adder;

PhysicsState::PhysicsState(const GUID& guid)
    : Encompasser(guid), _bodies(Vec2f::zeros(), 100.f, 10.f) {}

void PhysicsState::addBody(const Vec2f& centre, const Vec2f& extents, const Entity& entity) {
  _bodies.insert(centre, extents, entity);
}

void PhysicsState::addCollision(const Entity& entity, const CollisionInfo& info) {
  _collisions[entity].emplace_back(info);
}

void PhysicsState::clear() {
  _bodies.clear();
  _collisions.clear();
}

bool PhysicsState::aabbCast(const Vec2f& start,
                            const Vec2f& extents,
                            const Vec2f& dir,
                            float max_length,
                            QuadTree<Entity>::Hit& hit) const {
  return _bodies.aabbCast(start, extents, dir, max_length, hit);
}

bool PhysicsState::rayCast(const Vec2f& start,
                           const Vec2f& dir,
                           float max_length,
                           QuadTree<Entity>::Hit& hit) const {
  return _bodies.rayCast(start, dir, max_length, hit);
}

const std::vector<CollisionInfo>& PhysicsState::getCollisions(const Entity& entity) const {
  const static std::vector<CollisionInfo> empty;
  if (auto it = _collisions.find(entity); it != _collisions.end()) {
    return it->second;
  }
  return empty;
}