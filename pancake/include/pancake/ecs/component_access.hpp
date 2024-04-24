#pragma once

#include "ecs/common.hpp"

namespace pancake {
class ComponentAccess {
 public:
  ComponentAccess(const ComponentMask& reads = ComponentMask(),
                  const ComponentMask& writes = ComponentMask());

  const ComponentMask& getReads() const;
  const ComponentMask& getWrites() const;

  bool intersects(const ComponentAccess& other) const;
  bool subsets(const ComponentAccess& other) const;

  ComponentAccess operator&(const ComponentAccess& rhs) const;
  ComponentAccess operator|(const ComponentAccess& rhs) const;

  auto operator<=>(const ComponentAccess&) const = default;

  static const ComponentAccess empty;
  static const ComponentAccess full;

 private:
  ComponentMask _reads;
  ComponentMask _writes;
};
}  // namespace pancake