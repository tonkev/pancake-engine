#pragma once

#include "components/core.hpp"
#include "ecs/archetype.hpp"
#include "ecs/children_component_view.hpp"
#include "ecs/common.hpp"
#include "ecs/component_access.hpp"
#include "ecs/component_view.hpp"
#include "ecs/components.hpp"
#include "ecs/encompassers.hpp"
#include "ecs/message_boards.hpp"
#include "util/generational_array.hpp"
#include "util/guid.hpp"
#include "util/type_desc_library.hpp"
#include "util/type_id.hpp"

#include <optional>
#include <ranges>
#include <shared_mutex>
#include <type_traits>
#include <unordered_map>

namespace pancake {
class JSONObject;
class World {
 public:
  class EntityWrapper {
   public:
    EntityWrapper(const Entity& ent, const ComponentAccess& access, World& world);

    const Entity& entity() const;

    void destroy() const;

    bool isValid() const;

    void unparent() const;
    bool parentTo(const Entity& parent) const;
    bool parentTo(const EntityWrapper& parent) const;

    EntityWrapper createChild() const;

    Entity getArchetypeParent(const ComponentMask& required, const ComponentMask& one_of) const;

    template <typename T>
    T& addComponent() const {
      ensure(ComponentMask::full() == _access.getWrites());
      return *static_cast<T*>(_world.addComponent(_ent, TypeDescLibrary::get<T>(), nullptr));
    }

    template <typename T>
    T& addComponent(const T& value) const {
      ensure(ComponentMask::full() == _access.getWrites());
      return *static_cast<T*>(_world.addComponent(_ent, TypeDescLibrary::get<T>(), &value));
    }

    template <typename T>
    T& getComponent() const {
      if constexpr (std::is_const_v<T>) {
        ensure(_access.getReads().get(Components::getId(TypeDescLibrary::get<T>())) ||
               _access.getWrites().get(Components::getId(TypeDescLibrary::get<T>())));
      } else {
        ensure(_access.getWrites().get(Components::getId(TypeDescLibrary::get<T>())));
      }
      return *static_cast<T*>(_world.getComponent(_ent, TypeDescLibrary::get<T>()));
    }

    template <typename T>
    bool hasComponent() const {
      return _world.hasComponent<T>(_ent);
    }

    template <typename... Ts>
    bool hasComponents() const {
      return _world.hasComponents<Ts...>(_ent);
    }

    template <typename... Ts>
    std::optional<std::tuple<Ts*...>> getArchetypeParent() {
      ensure(Components::getAccess<const Base>().subsets(_access));
      return _world.getArchetypeParent<Ts...>(_ent);
    }

   private:
    const Entity _ent;
    const ComponentAccess& _access;
    World& _world;
  };

  World();
  World(const JSONObject& json);
  ~World();

  EntityWrapper createEntity();
  void destroyEntity(const Entity& ent);

  bool isValid(const Entity& ent) const;

  bool parentTo(const Entity& child, const Entity& parent);
  void unparent(const Entity& child);

  const Entity& getParent(const Entity& child);
  bool isChildOf(const Entity& child, const Entity& parent);

  EntityWrapper getEntityWrapper(const Entity& ent);

  template <typename T>
  T& addComponent(const Entity& ent) {
    return *static_cast<T*>(addComponent(ent, TypeDescLibrary::get<T>()));
  }

  template <typename T>
  T& addComponent(const Entity& ent, const T& value) {
    return *static_cast<T*>(addComponent(ent, TypeDescLibrary::get<T>(), &value));
  }

  template <typename T>
  T& getComponent(const Entity& ent) {
    return *static_cast<T*>(getComponent(ent, TypeDescLibrary::get<T>()));
  }

  template <typename T>
  bool hasComponent(const Entity& ent) {
    return hasComponent(ent, TypeDescLibrary::get<T>());
  }

  template <typename... Ts>
  bool hasComponents(const Entity& ent) {
    const ComponentMask mask = Components::getMask<Ts...>();
    return (mask & getComponentMask(ent)) == mask;
  }

  void* addComponent(const Entity& ent, const TypeDesc& desc, const void* value = nullptr);
  void removeComponent(const Entity& ent, const TypeDesc& desc);

  void* getComponent(const Entity& ent, const TypeDesc& desc);
  bool hasComponent(const Entity& ent, const TypeDesc& desc);

  const ComponentMask& getComponentMask(const Entity& ent) const;

  void* getOrAddComponent(const Entity& ent, const TypeDesc& desc);

  Entity getArchetypeParent(const Entity& child, const ComponentMask& mask);
  Entity getArchetypeParent(const Entity& child,
                            const ComponentMask& required,
                            const ComponentMask& one_of);

  template <typename... Ts>
  ComponentView::Formatter<Ts...> getComponents() {
    ComponentView* component_view = nullptr;

    const ComponentMask mask = Components::getMask<Ts...>();
    _component_views_mutex.lock_shared();
    if (!_component_views.contains(mask)) {
      _component_views_mutex.unlock_shared();
      _component_views_mutex.lock();
      component_view = _component_views.emplace(mask, new ComponentView(mask, *this)).first->second;
      _component_views_mutex.unlock();
    } else {
      component_view = _component_views.at(mask);
      _component_views_mutex.unlock_shared();
    }

    return component_view->get<Ts...>();
  }

  template <typename... Ts>
  ChildrenComponentView<Ts...> getChildrenComponents(const Entity& parent) {
    const ComponentMask mask = Components::getMask<Ts...>();
    return ChildrenComponentView<Ts...>(parent, *this);
  }

  template <typename... Ts>
  std::optional<std::tuple<Ts*...>> getArchetypeParent(const Entity& child) {
    const ComponentMask mask = Components::getMask<Ts...>();
    const Entity parent = getArchetypeParent(child, mask);

    if (Entity::null == parent) {
      return std::nullopt;
    } else {
      return std::tuple<Ts*...>{
          reinterpret_cast<Ts*>(getComponent(parent, TypeDescLibrary::get<Ts>()))...};
    }
  }

  void addEncompasser(std::unique_ptr<Encompasser>&& encompasser, const TypeId& id);
  void removeEncompasser(const Encompasser& encompasser, const TypeId& id);

  template <Encompassable T>
  void addEncompasser(std::unique_ptr<T>&& encompasser) {
    const TypeId& id = TypeId::get<T>();
    ensure(Encompassers::isEncompasser(id));

    addEncompasser(std::move(encompasser), id);
  }

  template <Encompassable T>
  void removeEncompasser(const T& encompasser) {
    const TypeId& id = TypeId::get<T>();
    ensure(Encompassers::isEncompasser(id));

    removeEncompasser(encompasser, id);
  }

  template <Encompassable T>
  auto getEncompassers() {
    const TypeId& id = TypeId::get<T>();
    ensure(Encompassers::isEncompasser(id));

    static const auto view = std::ranges::transform_view(
        [](const std::unique_ptr<Encompasser> ptr) { return static_cast<T&>(*ptr); });

    return _encompassers.at(id) | view;
  }

  template <Encompassable T>
  T* getEncompasser(const GUID& guid) {
    const TypeId& id = TypeId::get<T>();
    ensure(Encompassers::isEncompasser(id));

    auto& inner_map = _encompassers.at(id);
    if (inner_map.contains(guid)) {
      return static_cast<T*>(inner_map.at(guid).get());
    }

    return nullptr;
  }

  MessageBoards& getLocalMessages();

  void insert(const World& world, const Entity& dest = Entity::null);
  void clear();

  JSONObject asJson() const;

 private:
  using Archetypes = std::unordered_map<ComponentMask, Archetype*>;
  using ComponentDescIds = std::unordered_map<std::reference_wrapper<const TypeDesc>,
                                              ComponentId,
                                              std::hash<TypeDesc>,
                                              std::equal_to<TypeDesc>>;
  using EntityArchetypes = GenerationalArray<Archetype*, Entity::underlying_type, 2048>;

  EntityWrapper createEntity(const Entity& ent, const GUID& guid);
  Archetype& moveEntity(const Entity& ent, const ComponentMask& new_mask);

  const Archetypes& getArchetypes() const;

  Archetypes _archetypes;
  EntityArchetypes _entity_archetypes;

  friend ComponentView;

  std::unordered_map<ComponentMask, ComponentView*> _component_views;
  std::shared_mutex _component_views_mutex;

  std::unordered_map<std::reference_wrapper<const TypeId>,
                     std::unordered_map<GUID, std::unique_ptr<Encompasser>>,
                     std::hash<TypeId>,
                     std::equal_to<TypeId>>
      _encompassers;

  MessageBoards _local_messages;
};

using EntityWrapper = World::EntityWrapper;
}  // namespace pancake