#include "ecs/world.hpp"

#include "components/core.hpp"
#include "ecs/components.hpp"
#include "ecs/encompasser.hpp"
#include "util/assert.hpp"
#include "util/componentify_json.hpp"
#include "util/fewi.hpp"
#include "util/jsonify_component.hpp"

#include <algorithm>

using namespace pancake;

World::EntityWrapper::EntityWrapper(const Entity& ent, const ComponentAccess& access, World& world)
    : _ent(ent), _access(access), _world(world) {}

const Entity& World::EntityWrapper::entity() const {
  return _ent;
}

void World::EntityWrapper::destroy() const {
  ensure(ComponentMask::full() == _access.getWrites());
  _world.destroyEntity(_ent);
}

bool World::EntityWrapper::isValid() const {
  ensure(ComponentAccess::empty != _access);
  return _world.isValid(_ent);
}

void World::EntityWrapper::unparent() const {
  ensure(_access.getWrites().get(Components::getId(TypeDescLibrary::get<Base>())));
  _world.unparent(_ent);
}

bool World::EntityWrapper::parentTo(const Entity& parent) const {
  ensure(_access.getWrites().get(Components::getId(TypeDescLibrary::get<Base>())));
  return _world.parentTo(_ent, parent);
}

bool World::EntityWrapper::parentTo(const EntityWrapper& parent) const {
  ensure(_access.getWrites().get(Components::getId(TypeDescLibrary::get<Base>())));
  return _world.parentTo(_ent, parent._ent);
}

EntityWrapper World::EntityWrapper::createChild() const {
  ensure(ComponentMask::full() == _access.getWrites());
  EntityWrapper child = _world.createEntity();
  child.parentTo(*this);
  return child;
}

Entity World::EntityWrapper::getArchetypeParent(const ComponentMask& required,
                                                const ComponentMask& one_of) const {
  ensure(Components::getAccess<const Base>().subsets(_access));
  return _world.getArchetypeParent(_ent, required, one_of);
}

World::World() : _local_messages(false) {
  const ComponentMask base_mask = Components::getMask<Base>();
  _archetypes.emplace(base_mask, new Archetype(*this, base_mask));

  for (const TypeId& tid : Encompassers::getDescs()) {
    _encompassers[tid];
  }
}

World::World(const JSONObject& json) : World() {
  const TypeDesc& base_desc = TypeDescLibrary::get<Base>();
  for (const auto& [ent_guid, ent_json] : json.pairs()) {
    if (const JSONObject* ent_obj = ent_json->asObject(); (nullptr != ent_obj)) {
      if (const JSONObject* base_obj = ent_obj->get<JSONObject>("Base"); (nullptr != base_obj)) {
        Base base;
        base_desc.visit(ComponentifyJSON(&base, *base_obj));
        EntityWrapper ent = createEntity(base.self, base.guid);
        for (const auto& [comp_name, comp_json] : ent_obj->pairs()) {
          if (const TypeDesc* comp_desc = TypeDescLibrary::get(comp_name);
              (nullptr != comp_desc) && Components::isComponent(*comp_desc)) {
            comp_desc->visit(
                ComponentifyJSON(getOrAddComponent(ent.entity(), *comp_desc), *comp_json));
          }
        }
      }
    }
  }
}

World::~World() {
  for (const auto& [_, archetype] : _archetypes) {
    delete archetype;
  }
  for (const auto& [_, comp_view] : _component_views) {
    delete comp_view;
  }
}

EntityWrapper World::createEntity() {
  Archetype* arch = _archetypes[Components::getMask<Base>()];
  const Entity ent = _entity_archetypes.insert(arch);

  Base base;
  base.self = ent;
  base.guid = GUID::gen();
  arch->setComponent(arch->add(ent), TypeDescLibrary::get<Base>(), &base);

  return {ent, ComponentAccess::full, *this};
}

// No protection against ent.id already being active!
EntityWrapper World::createEntity(const Entity& ent, const GUID& guid) {
  Archetype* arch = _archetypes[Components::getMask<Base>()];
  _entity_archetypes.insert(ent, arch);

  Base base;
  base.self = ent;
  base.guid = guid;
  arch->setComponent(arch->add(ent), TypeDescLibrary::get<Base>(), &base);

  return {ent, ComponentAccess::full, *this};
}

void World::destroyEntity(const Entity& ent) {
  ensure(_entity_archetypes.has(ent));
  Archetype* arch = _entity_archetypes[ent];
  arch->remove(arch->getArchetypeId(ent));
  _entity_archetypes.remove(ent);

  const ComponentMask& mask = arch->mask();
  for (auto& [view_mask, view] : _component_views) {
    if (ComponentMask::empty() != (view_mask & mask)) {
      view->invalidate();
    }
  }
}

bool World::isValid(const Entity& ent) const {
  return _entity_archetypes.has(ent);
}

bool World::parentTo(const Entity& child, const Entity& parent) {
  if (isChildOf(child, parent)) {
    return true;
  }
  if (!((child == parent) || isChildOf(parent, child))) {
    unparent(child);
    Base& base = getComponent<Base>(child);
    base.parent = parent;
    Base& parent_base = getComponent<Base>(parent);
    if (Entity::null == parent_base.first_child) {
      parent_base.first_child = child;
    } else {
      Entity sibling = parent_base.first_child;
      while (Entity::null != sibling) {
        Base& sibling_base = getComponent<Base>(sibling);
        sibling = sibling_base.next_sibling;
        if (Entity::null == sibling_base.next_sibling) {
          sibling_base.next_sibling = child;
        }
      }
    }
    return true;
  }
  return false;
}

void World::unparent(const Entity& child) {
  if (Entity::null != child) {
    Base& base = getComponent<Base>(child);
    if (Entity::null != base.parent) {
      Base& parent_base = getComponent<Base>(base.parent);
      if (parent_base.first_child == child) {
        parent_base.first_child = base.next_sibling;
      } else {
        Entity sibling = parent_base.first_child;
        while (sibling != child) {
          Base& sibling_base = getComponent<Base>(sibling);
          sibling = sibling_base.next_sibling;
          if (sibling_base.next_sibling == child) {
            sibling_base.next_sibling = base.next_sibling;
          }
        }
      }
    }
    base.parent = Entity::null;
    base.next_sibling = Entity::null;
  }
}

const Entity& World::getParent(const Entity& child) {
  return getComponent<Base>(child).parent;
}

bool World::isChildOf(const Entity& child, const Entity& parent) {
  Entity ent = child;
  while (Entity::null != ent) {
    const Base& base = getComponent<Base>(ent);
    if (base.parent == parent) {
      return true;
    }
    ent = base.parent;
  }
  return false;
}

EntityWrapper World::getEntityWrapper(const Entity& ent) {
  ensure(_entity_archetypes.has(ent));
  return EntityWrapper(ent, ComponentAccess::full, *this);
}

Archetype& World::moveEntity(const Entity& ent, const ComponentMask& new_mask) {
  ensure(_entity_archetypes.has(ent));

  Archetype& prev_arch = *_entity_archetypes[ent];
  const ComponentMask& prev_mask = prev_arch.mask();
  const ArchetypeId prev_arch_id = prev_arch.getArchetypeId(ent);

  if (!_archetypes.contains(new_mask)) {
    _archetypes.emplace(new_mask, new Archetype(*this, new_mask));
  }

  Archetype& new_arch = *_archetypes.at(new_mask);
  const ArchetypeId new_arch_id = new_arch.add(ent);

  for (ComponentId comp_id : (prev_mask & new_mask)) {
    const TypeDesc& comp_desc = Components::getDesc(comp_id);
    new_arch.setComponent(new_arch_id, comp_desc, prev_arch.getComponent(prev_arch_id, comp_desc));
  }

  prev_arch.remove(prev_arch_id);
  _entity_archetypes[ent] = &new_arch;

  const ComponentMask& combined_mask = prev_mask | new_mask;
  for (auto& [view_mask, view] : _component_views) {
    if (ComponentMask::empty() != (view_mask & combined_mask)) {
      view->invalidate();
    }
  }

  return new_arch;
}

void* World::addComponent(const Entity& ent, const TypeDesc& desc, const void* value) {
  ensure(_entity_archetypes.has(ent));

  const ComponentMask& prev_mask = _entity_archetypes[ent]->mask();
  ensure(!prev_mask.get(Components::getId(desc)));

  const ComponentMask new_mask = prev_mask.with(Components::getId(desc));
  Archetype& new_arch = moveEntity(ent, new_mask);

  return new_arch.setComponent(new_arch.getArchetypeId(ent), desc, value);
}

void World::removeComponent(const Entity& ent, const TypeDesc& desc) {
  ensure(_entity_archetypes.has(ent));
  ensure(TypeDescLibrary::get<Base>() != desc);

  const ComponentMask& prev_mask = _entity_archetypes[ent]->mask();
  ensure(prev_mask.get(Components::getId(desc)));

  const ComponentMask new_mask = prev_mask.without(Components::getId(desc));
  Archetype& new_arch = moveEntity(ent, new_mask);
}

void* World::getComponent(const Entity& ent, const TypeDesc& desc) {
  ensure(_entity_archetypes.has(ent));
  ensure(Components::isComponent(desc));

  Archetype* arch = _entity_archetypes[ent];
  return arch->getComponent(arch->getArchetypeId(ent), desc);
}

bool World::hasComponent(const Entity& ent, const TypeDesc& desc) {
  ensure(_entity_archetypes.has(ent));
  ensure(Components::isComponent(desc));

  return _entity_archetypes[ent]->mask().get(Components::getId(desc));
}

const ComponentMask& World::getComponentMask(const Entity& ent) const {
  if (_entity_archetypes.has(ent)) {
    return _entity_archetypes[ent]->mask();
  }
  return ComponentMask::empty();
}

void* World::getOrAddComponent(const Entity& ent, const TypeDesc& desc) {
  if (hasComponent(ent, desc)) {
    return getComponent(ent, desc);
  }
  return addComponent(ent, desc);
}

Entity World::getArchetypeParent(const Entity& child, const ComponentMask& mask) {
  ensure((Entity::null == child) || _entity_archetypes.has(child));

  Entity ent = child;
  while ((Entity::null != ent) && ((mask & _entity_archetypes[ent]->mask()) != mask)) {
    ent = getParent(ent);
  }

  return ent;
}

Entity World::getArchetypeParent(const Entity& child,
                                 const ComponentMask& required,
                                 const ComponentMask& one_of) {
  ensure((Entity::null == child) || _entity_archetypes.has(child));

  Entity ent = child;
  while ((Entity::null != ent) && ((required & _entity_archetypes[ent]->mask()) != required) &&
         ((one_of & _entity_archetypes[ent]->mask()) != ComponentMask::empty())) {
    ent = getParent(ent);
  }

  return ent;
}

void World::addEncompasser(std::unique_ptr<Encompasser>&& encompasser, const TypeId& id) {
  ensure(Encompassers::isEncompasser(id));
  ensure(_encompassers.contains(id));

  if (Encompasser* enc = encompasser.release(); (nullptr != enc)) {
    _encompassers.at(id).emplace(enc->guid(), enc);
  }
}

void World::removeEncompasser(const Encompasser& encompasser, const TypeId& id) {
  ensure(Encompassers::isEncompasser(id));
  ensure(_encompassers.contains(id));

  if (_encompassers.contains(id)) {
    auto& desc_encompassers = _encompassers.at(id);
    if (const auto it = desc_encompassers.find(encompasser.guid());
        (it != desc_encompassers.end())) {
      desc_encompassers.erase(it);
    }
  }
}

MessageBoards& World::getLocalMessages() {
  return _local_messages;
}

void World::insert(const World& world, const Entity& dest) {
  const Components& components = Components::get();
  std::unordered_map<Entity, Entity> transfer_map = {{Entity::null, Entity::null}};

  for (const auto& [ent, arch] : world._entity_archetypes) {
    if (!transfer_map.contains(ent)) {
      transfer_map.emplace(ent, createEntity().entity());
    }
    const Entity& new_ent = transfer_map.at(ent);

    for (const ComponentId comp_id : arch->mask()) {
      const TypeDesc& comp_desc = Components::getDesc(comp_id);

      void* new_comp = nullptr;
      if (comp_desc == TypeDescLibrary::get<Base>()) {
        new_comp = getComponent(new_ent, comp_desc);
      } else {
        new_comp = addComponent(new_ent, comp_desc,
                                arch->getComponent(arch->getArchetypeId(ent), comp_desc));
      }

      const auto offset_to_ent_ref = [new_comp](size_t offset) -> Entity& {
        return *reinterpret_cast<Entity*>(static_cast<char*>(new_comp) + offset);
      };

      for (Entity& ent_member :
           components.getEntityMemberOffsets(comp_id) | std::views::transform(offset_to_ent_ref)) {
        if (!transfer_map.contains(ent_member)) {
          transfer_map.emplace(ent_member, createEntity().entity());
        }
        ent_member = transfer_map.at(ent_member);
      }
    }

    if ((Entity::null != dest) && (Entity::null == getParent(new_ent))) {
      parentTo(new_ent, dest);
    }
  }
}

void World::clear() {
  for (const auto& [_, archetype] : _archetypes) {
    archetype->clear();
  }
  for (const auto& [_, comp_view] : _component_views) {
    comp_view->invalidate();
  }
  _entity_archetypes.clear();
  _local_messages.clear();
}

const World::Archetypes& World::getArchetypes() const {
  return _archetypes;
}

JSONObject World::asJson() const {
  JSONObject doc;

  for (const auto& [ent, arch] : _entity_archetypes) {
    ArchetypeId arch_id = arch->getArchetypeId(ent);
    const Base* base =
        static_cast<const Base*>(arch->getComponent(arch_id, TypeDescLibrary::get<Base>()));

    JSONObject& ent_obj = doc.getOrCreate<JSONObject>(base->guid.hex());
    for (ComponentId comp_id : arch->mask()) {
      const TypeDesc& comp_desc = Components::getDesc(comp_id);
      comp_desc.visit(
          JSONifyComponent(ent_obj, comp_desc.name(), arch->getComponent(arch_id, comp_desc)));
    }
  }

  return doc;
}