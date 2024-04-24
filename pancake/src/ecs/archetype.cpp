#include "ecs/archetype.hpp"

#include "ecs/components.hpp"
#include "ecs/world.hpp"
#include "util/assert.hpp"

#include <cstring>

using namespace pancake;

Archetype::Archetype(const World& world, const ComponentMask& mask) : _mask(mask), _stride(0) {
  for (const ComponentId& comp_id : mask) {
    const TypeDesc& comp_desc = Components::getDesc(comp_id);
    _offsets.emplace(comp_desc, _stride);
    _stride += comp_desc.size();
  }
}

// components need to be moved via getComponent/setComponent!
ArchetypeId Archetype::add(const Entity& ent) {
  ensure(!_entity_archetype_ids.contains(ent));

  const ArchetypeId arch_id = static_cast<ArchetypeId>(_entity_archetype_ids.size());
  _entity_archetype_ids.emplace(ent, arch_id);

  const size_t min_size = _stride * (static_cast<size_t>(arch_id) + 1);
  if (_pool.size() < min_size) {
    _pool.resize(min_size);
  }

  return arch_id;
}

void Archetype::remove(ArchetypeId arch_id) {
  ensure(arch_id < _entity_archetype_ids.size());

  ArchetypeId count = static_cast<ArchetypeId>(_entity_archetype_ids.size());
  std::erase_if(_entity_archetype_ids,
                [arch_id](const auto& pair) { return pair.second == arch_id; });

  for (const auto& [desc, offset] : _offsets) {
    desc.get().destroy(&_pool[(_stride * arch_id) + offset]);
  }

  if (arch_id + 1 < count) {
    std::memmove(&_pool[_stride * arch_id], &_pool[_stride * (static_cast<size_t>(arch_id) + 1)],
                 _stride * (static_cast<size_t>(count) - arch_id));

    for (auto& [_, arch_id2] : _entity_archetype_ids) {
      if (arch_id < arch_id2) {
        --arch_id2;
      }
    }
  }
}

ArchetypeId Archetype::getArchetypeId(const Entity& ent) const {
  ensure(_entity_archetype_ids.contains(ent));
  return _entity_archetype_ids.at(ent);
}

void* Archetype::getComponent(ArchetypeId arch_id, const TypeDesc& desc) {
  ensure(arch_id < _entity_archetype_ids.size());
  ensure(_offsets.contains(desc));
  return &_pool[(_stride * arch_id) + _offsets.at(desc)];
}

const void* Archetype::getComponent(ArchetypeId arch_id, const TypeDesc& desc) const {
  ensure(arch_id < _entity_archetype_ids.size());
  ensure(_offsets.contains(desc));
  return &_pool[(_stride * arch_id) + _offsets.at(desc)];
}

void* Archetype::setComponent(ArchetypeId arch_id, const TypeDesc& desc, const void* comp) {
  ensure(arch_id < _entity_archetype_ids.size());
  return std::memcpy(getComponent(arch_id, desc), nullptr == comp ? desc.default_value() : comp,
                     desc.size());
}

void Archetype::clear() {
  _pool.clear();
  _entity_archetype_ids.clear();
}

ArchetypeId Archetype::size() const {
  return static_cast<ArchetypeId>(_entity_archetype_ids.size());
}

const ComponentMask& Archetype::mask() const {
  return _mask;
}