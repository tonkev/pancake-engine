#pragma once

#include "ecs/common.hpp"

namespace pancake {
class EncompasserAccess {
 public:
  EncompasserAccess(const EncompasserMask& reads = EncompasserMask(),
                    const EncompasserMask& writes = EncompasserMask());

  const EncompasserMask& getReads() const;
  const EncompasserMask& getWrites() const;

  bool intersects(const EncompasserAccess& other) const;
  bool subsets(const EncompasserAccess& other) const;

  EncompasserAccess operator&(const EncompasserAccess& rhs) const;
  EncompasserAccess operator|(const EncompasserAccess& rhs) const;

  auto operator<=>(const EncompasserAccess&) const = default;

  static const EncompasserAccess empty;
  static const EncompasserAccess full;

 private:
  EncompasserMask _reads;
  EncompasserMask _writes;
};
}  // namespace pancake