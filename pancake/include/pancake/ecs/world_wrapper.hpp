#pragma once

#include "ecs/world.hpp"
#include "util/assert.hpp"

#include <source_location>

namespace pancake {
template <typename Cache, class Parent>
class CachingSystem;
class GUID;

class WorldWrapper {
 public:
  WorldWrapper(World& world,
               const ComponentAccess& comp_access,
               const EncompasserAccess& enc_access);

  EntityWrapper createEntity() const;
  EntityWrapper getEntityWrapper(const Entity& ent) const;

  template <typename T>
  bool hasComponent(const Entity& ent) const {
    return _world.hasComponent<T>(ent);
  }

  template <typename... Ts>
  ComponentView::Formatter<Ts...> getComponents(
      std::source_location location = std::source_location::current()) const {
    ensureLoc(Components::getAccess<Ts...>().subsets(_comp_access), location);
    return _world.getComponents<Ts...>();
  }

  template <typename... Ts>
  ChildrenComponentView<Ts...> getChildrenComponents(
      const Entity& parent,
      std::source_location location = std::source_location::current()) const {
    ensureLoc(Components::getAccess<Ts...>().subsets(_comp_access), location);
    return _world.getChildrenComponents<Ts...>(parent);
  }

  template <Encompassable T>
  auto getEncompassers(std::source_location location = std::source_location::current()) const {
    ensureLoc(Encompassers::getAccess<T>().subsets(_enc_access), location);
    return _world.getEncompassers<T>();
  }

  template <Encompassable T>
  T* getEncompasser(const GUID& guid,
                    std::source_location location = std::source_location::current()) const {
    ensureLoc(Encompassers::getAccess<T>().subsets(_enc_access), location);
    return _world.getEncompasser<T>(guid);
  }

  template <Encompassable T>
  void addEncompasser(std::unique_ptr<T>&& encompasser,
                      std::source_location location = std::source_location::current()) const {
    const TypeId& id = TypeId::get<T>();
    ensureLoc(_enc_access.getWrites().get(Encompassers::getId(id)), location);
    _world.addEncompasser<T>(std::move(encompasser));
  }

  template <Encompassable T>
  void removeEncompasser(const T& encompasser,
                         std::source_location location = std::source_location::current()) const {
    const TypeId& id = TypeId::get<T>();
    ensureLoc(_enc_access.getWrites().get(Encompassers::getId(id)), location);
    _world.removeEncompasser(encompasser);
  }

 protected:
  const World& world() const;

  template <typename Cache, class Parent>
  friend class CachingSystem;

 private:
  World& _world;
  const ComponentAccess& _comp_access;
  const EncompasserAccess& _enc_access;
};
}  // namespace pancake