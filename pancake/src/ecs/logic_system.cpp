#include "ecs/logic_system.hpp"

using namespace pancake;

LogicSystem::Creators& LogicSystem::getCreators() {
  static Creators creators;
  return creators;
}

Ptr<Systems> LogicSystem::createLogicSystems(Session& session) {
  const Creators& creators = getCreators();

  Ptr<Systems> systems(new Systems());
  systems->reserve(systems->size() + creators.size());
  for (const auto& creator : creators) {
    systems->emplace_back(creator(session));
  }

  return systems;
}