#pragma once

#include "ecs/common.hpp"
#include "util/type_desc.hpp"

#include <unordered_map>
#include <vector>

namespace pancake {
class World;

// manages component pool for entities with matching component sets
class Archetype {
 public:
  Archetype(const World& world, const ComponentMask& mask);
  Archetype(const Archetype&) = delete;
  ~Archetype() = default;

  ArchetypeId add(const Entity& ent);
  void remove(ArchetypeId arch_id);

  ArchetypeId getArchetypeId(const Entity& ent) const;

  void* getComponent(ArchetypeId arch_id, const TypeDesc& desc);
  const void* getComponent(ArchetypeId arch_id, const TypeDesc& desc) const;

  void* setComponent(ArchetypeId arch_id, const TypeDesc& desc, const void* comp = nullptr);

  void clear();

  ArchetypeId size() const;
  const ComponentMask& mask() const;

 private:
  using OffsetMap = std::unordered_map<std::reference_wrapper<const TypeDesc>,
                                       size_t,
                                       std::hash<TypeDesc>,
                                       std::equal_to<TypeDesc>>;

  const ComponentMask _mask;
  size_t _stride;
  std::vector<char> _pool;
  OffsetMap _offsets;
  std::unordered_map<Entity, ArchetypeId> _entity_archetype_ids;
};
}  // namespace pancake