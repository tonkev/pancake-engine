#pragma once

#include "ecs/common.hpp"
#include "ecs/component_access.hpp"
#include "util/type_desc_library.hpp"

#include <span>
#include <unordered_map>
#include <vector>

namespace pancake {
class Components {
 public:
  using ComponentTypeDescs = std::span<std::reference_wrapper<const TypeDesc>>;

  template <typename... Ts>
  static ComponentMask getMask() {
    ComponentMask mask = ComponentMask();
    const ComponentId ids[] = {getId(TypeDescLibrary::get<Ts>())...};
    for (const ComponentId id : ids) {
      mask.set(id);
    }
    return mask;
  }

  template <typename... Ts>
  static ComponentAccess getAccess() {
    const TypeDesc* read_descs[] = {std::is_const_v<Ts> ? &TypeDescLibrary::get<Ts>() : nullptr...};
    const TypeDesc* write_descs[] = {(!std::is_const_v<Ts>) ? &TypeDescLibrary::get<Ts>()
                                                            : nullptr...};

    ComponentMask read_mask = ComponentMask();
    for (const TypeDesc* read_desc : read_descs) {
      if (nullptr == read_desc) {
        continue;
      }
      read_mask.set(getId(*read_desc));
    }

    ComponentMask write_mask = ComponentMask();
    for (const TypeDesc* write_desc : write_descs) {
      if (nullptr == write_desc) {
        continue;
      }
      write_mask.set(getId(*write_desc));
    }

    return ComponentAccess(read_mask, write_mask);
  }

  static ComponentId getId(const TypeDesc& desc);
  static const TypeDesc& getDesc(ComponentId comp_id);
  static bool isComponent(const TypeDesc& desc);

  static Components& get();

  ComponentId add(const TypeDesc& desc);
  void add(const ComponentTypeDescs& descs);
  std::span<const size_t> getEntityMemberOffsets(ComponentId comp_id) const;

 private:
  Components() = default;

  ComponentId _getId(const TypeDesc& desc) const;
  const TypeDesc& _getDesc(ComponentId comp_id) const;
  bool _isComponent(const TypeDesc& desc) const;

  using ComponentDescIds = std::unordered_map<std::reference_wrapper<const TypeDesc>,
                                              ComponentId,
                                              std::hash<TypeDesc>,
                                              std::equal_to<TypeDesc>>;

  struct ComponentInfo {
    ComponentInfo(const TypeDesc& desc);

    const TypeDesc& desc;
    std::vector<size_t> entity_member_offsets;
  };

  std::vector<ComponentInfo> _component_infos;
  ComponentDescIds _component_desc_ids;
};
}  // namespace pancake