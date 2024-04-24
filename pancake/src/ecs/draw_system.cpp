#include "ecs/draw_system.hpp"

using namespace pancake;

DrawSystem::Creators& DrawSystem::getCreators() {
  static Creators creators;
  return creators;
}

Ptr<Systems> DrawSystem::createDrawSystems(Session& session) {
  const Creators& creators = getCreators();

  Ptr<Systems> systems(new Systems());
  systems->reserve(systems->size() + creators.size());
  for (const auto& creator : creators) {
    systems->emplace_back(creator(session));
  }

  return systems;
}