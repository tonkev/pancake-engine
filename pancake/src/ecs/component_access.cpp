#include "ecs/component_access.hpp"

using namespace pancake;

const ComponentAccess ComponentAccess::empty{};

const ComponentAccess ComponentAccess::full(ComponentMask::full(), ComponentMask::full());

ComponentAccess::ComponentAccess(const ComponentMask& reads, const ComponentMask& writes)
    : _reads(reads), _writes(writes) {}

const ComponentMask& ComponentAccess::getReads() const {
  return _reads;
}

const ComponentMask& ComponentAccess::getWrites() const {
  return _writes;
}

bool ComponentAccess::intersects(const ComponentAccess& other) const {
  return ((_reads & other._writes) != ComponentMask::empty()) ||
         ((other._reads & _writes) != ComponentMask::empty()) ||
         ((_writes & other._writes) != ComponentMask::empty());
}

bool ComponentAccess::subsets(const ComponentAccess& super_set) const {
  ComponentMask super_read_writes = super_set._reads | super_set._writes;
  return ((_reads | super_read_writes) == super_read_writes) &&
         ((_writes | super_set._writes) == super_set._writes);
}

ComponentAccess ComponentAccess::operator&(const ComponentAccess& rhs) const {
  return ComponentAccess(_reads & rhs._reads, _writes & rhs._writes);
}

ComponentAccess ComponentAccess::operator|(const ComponentAccess& rhs) const {
  return ComponentAccess(_reads | rhs._reads, _writes | rhs._writes);
}
