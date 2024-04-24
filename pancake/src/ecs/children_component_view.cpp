#include "ecs/children_component_view.hpp"

#include "components/core.hpp"
#include "ecs/world.hpp"

using namespace pancake;

BaseChildrenComponentView::Iterator::Iterator(const ComponentMask& mask,
                                              const Entity& child,
                                              World& world)
    : _mask(mask), _child(child), _world(world) {
  if ((_mask & _world.getComponentMask(_child)) != _mask) {
    ++(*this);
  }
}

BaseChildrenComponentView::Iterator& BaseChildrenComponentView::Iterator::operator++() {
  while (Entity::null != _child) {
    _child = _world.getComponent<Base>(_child).next_sibling;
    if ((_mask & _world.getComponentMask(_child)) == _mask) {
      fill();
      break;
    }
  }
  return *this;
}

bool BaseChildrenComponentView::Iterator::operator==(
    const BaseChildrenComponentView::Iterator& rhs) const {
  return (_child == rhs._child) && (&_world == &rhs._world);
}

void* BaseChildrenComponentView::Iterator::getComponent(const TypeDesc& desc) const {
  if (Entity::null == _child) {
    return nullptr;
  }
  return _world.getComponent(_child, desc);
}

BaseChildrenComponentView::BaseChildrenComponentView(const Entity& parent, World& world)
    : _parent(parent), _world(world) {}

const Entity& BaseChildrenComponentView::getFirstChild() const {
  if (Entity::null == _parent) {
    return Entity::null;
  }
  return _world.getComponent<Base>(_parent).first_child;
}