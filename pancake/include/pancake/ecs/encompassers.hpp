#pragma once

#include "ecs/common.hpp"
#include "ecs/encompasser_access.hpp"
#include "util/type_id.hpp"

#include <concepts>
#include <unordered_map>
#include <vector>

namespace pancake {
class Encompasser;

template <typename T>
concept Encompassable = std::is_base_of_v<Encompasser, T>;

class Encompassers {
 public:
  using EncompasserDescs = std::vector<std::reference_wrapper<const TypeId>>;

  template <Encompassable T>
  class StaticAdder {
   public:
    StaticAdder() { Encompassers::get().add(TypeId::get<T>()); }
  };

  template <typename... Ts>
  static EncompasserMask getMask() {
    EncompasserMask mask = EncompasserMask();
    const EncompasserId ids[] = {getId(TypeId::get<Ts>())...};
    for (const EncompasserId id : ids) {
      mask.set(id);
    }
    return mask;
  }

  template <typename... Ts>
  static EncompasserAccess getAccess() {
    const TypeId* read_descs[] = {std::is_const_v<Ts> ? &TypeId::get<Ts>() : nullptr...};
    const TypeId* write_descs[] = {(!std::is_const_v<Ts>) ? &TypeId::get<Ts>() : nullptr...};

    EncompasserMask read_mask = EncompasserMask();
    for (const TypeId* read_desc : read_descs) {
      if (nullptr == read_desc) {
        continue;
      }
      read_mask.set(getId(*read_desc));
    }

    EncompasserMask write_mask = EncompasserMask();
    for (const TypeId* write_desc : write_descs) {
      if (nullptr == write_desc) {
        continue;
      }
      write_mask.set(getId(*write_desc));
    }

    return EncompasserAccess(read_mask, write_mask);
  }

  static EncompasserId getId(const TypeId& desc);
  static const TypeId& getTypeId(EncompasserId enc_id);
  static const EncompasserDescs& getDescs();

  static bool isEncompasser(const TypeId& desc);

 protected:
  EncompasserId add(const TypeId& desc);

  EncompasserId _getId(const TypeId& desc) const;
  const TypeId& _getTypeId(EncompasserId enc_id) const;
  const EncompasserDescs& _getDescs() const;

  bool _isEncompasser(const TypeId& desc) const;

  static Encompassers& get();

  template <Encompassable T>
  friend class StaticAdder;

 private:
  Encompassers() = default;

  using EncompasserIds = std::unordered_map<std::reference_wrapper<const TypeId>,
                                            EncompasserId,
                                            std::hash<TypeId>,
                                            std::equal_to<TypeId>>;

  EncompasserDescs _encompasser_descs;
  EncompasserIds _encompasser_desc_ids;
};
}  // namespace pancake