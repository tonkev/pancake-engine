#include "ecs/encompasser_access.hpp"

using namespace pancake;

const EncompasserAccess EncompasserAccess::empty{};

const EncompasserAccess EncompasserAccess::full(EncompasserMask::full(), EncompasserMask::full());

EncompasserAccess::EncompasserAccess(const EncompasserMask& reads, const EncompasserMask& writes)
    : _reads(reads), _writes(writes) {}

const EncompasserMask& EncompasserAccess::getReads() const {
  return _reads;
}

const EncompasserMask& EncompasserAccess::getWrites() const {
  return _writes;
}

bool EncompasserAccess::intersects(const EncompasserAccess& other) const {
  return ((_reads & other._writes) != EncompasserMask::empty()) ||
         ((other._reads & _writes) != EncompasserMask::empty()) ||
         ((_writes & other._writes) != EncompasserMask::empty());
}

bool EncompasserAccess::subsets(const EncompasserAccess& super_set) const {
  EncompasserMask super_read_writes = super_set._reads | super_set._writes;
  return ((_reads | super_read_writes) == super_read_writes) &&
         ((_writes | super_set._writes) == super_set._writes);
}

EncompasserAccess EncompasserAccess::operator&(const EncompasserAccess& rhs) const {
  return EncompasserAccess(_reads & rhs._reads, _writes & rhs._writes);
}

EncompasserAccess EncompasserAccess::operator|(const EncompasserAccess& rhs) const {
  return EncompasserAccess(_reads | rhs._reads, _writes | rhs._writes);
}
