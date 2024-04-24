#pragma once

#include "ecs/common.hpp"
#include "ecs/components.hpp"
#include "util/type_desc_library.hpp"

namespace pancake {
class World;

class BaseChildrenComponentView {
 public:
  struct Iterator {
   public:
    Iterator(const ComponentMask& mask, const Entity& child, World& world);

    Iterator& operator++();

    bool operator==(const Iterator& rhs) const;

   protected:
    virtual void fill() = 0;

    void* getComponent(const TypeDesc& desc) const;

   private:
    ComponentMask _mask;
    Entity _child;
    World& _world;
  };

 protected:
  BaseChildrenComponentView(const Entity& parent, World& world);

  const Entity& getFirstChild() const;

  Entity _parent;
  World& _world;
};

template <typename... Ts>
class ChildrenComponentView : BaseChildrenComponentView {
 public:
  struct Iterator : BaseChildrenComponentView::Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<Ts*...>;
    using pointer = const std::tuple<Ts*...>*;
    using reference = const std::tuple<Ts*...>&;

    Iterator(const ComponentMask& mask, const Entity& child, World& world)
        : BaseChildrenComponentView::Iterator(mask, child, world),
          _view({static_cast<Ts*>(nullptr)...}) {
      fill();
    }

    reference operator*() { return _view; }

    pointer operator->() { return _view; }

    Iterator& operator++() {
      BaseChildrenComponentView::Iterator::operator++();
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const Iterator& rhs) {
      return BaseChildrenComponentView::Iterator::operator==(rhs);
    };

    bool operator!=(const Iterator& rhs) { return !(*this == rhs); };

   private:
    virtual void fill() override {
      _view = {reinterpret_cast<Ts*>(getComponent(TypeDescLibrary::get<Ts>()))...};
    }

    std::tuple<Ts*...> _view;
  };

  ChildrenComponentView(const Entity& parent, World& world)
      : BaseChildrenComponentView(parent, world), _mask(Components::getMask<Ts...>()) {}

  Iterator begin() const { return Iterator(_mask, getFirstChild(), _world); }
  Iterator end() const { return Iterator(_mask, Entity::null, _world); }

 private:
  ComponentMask _mask;
};
}  // namespace pancake