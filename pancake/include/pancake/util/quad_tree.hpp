#pragma once

#include "util/aabb.hpp"
#include "util/matrix.hpp"

#include <array>
#include <concepts>
#include <memory>
#include <vector>

namespace pancake {
class BaseQuadTree {
 protected:
  struct BaseHit {
    Vec2f hit = Vec2f::zeros();
    Vec2f normal = Vec2f::zeros();
    float length = std::numeric_limits<float>::infinity();
  };

  struct ElementInfo {
    Vec2f centre;
    Vec2f extents;
  };

  BaseQuadTree(const Vec2f& centre, float size, float min_size);

  bool aabbCast(const Vec2f& box_centre,
                const Vec2f& box_extents,
                const Vec2f& dir,
                float max_length,
                BaseHit& hit) const;
  bool aabbCast(const Vec2f& sweep_centre,
                const Vec2f& sweep_extents,
                const Vec2f& box_centre,
                const Vec2f& box_extents,
                const Vec2f& dir,
                const Vec2f& gradients,
                BaseHit& hit) const;

  bool rayCast(const Vec2f& start, const Vec2f& dir, float max_length, BaseHit& hit) const;

  BaseQuadTree& getDestinationNode(const Vec2f& centre, const Vec2f& extents);

  virtual std::unique_ptr<BaseQuadTree> moveToNewNode() = 0;
  virtual std::unique_ptr<BaseQuadTree> createNode(const Vec2f& centre, float size) = 0;

  virtual void setElement(BaseHit& hit, size_t i) const = 0;

  Vec2f _centre;
  float _size;
  float _min_size;

  std::array<std::unique_ptr<BaseQuadTree>, 4> _children;
  std::vector<ElementInfo> _element_infos;
};

template <typename T>
class QuadTree : public BaseQuadTree {
 public:
  struct Hit : public BaseHit {
    T element;
  };

  QuadTree(const Vec2f& centre, float size, float min_size)
      : BaseQuadTree(centre, size, min_size) {}

  void insert(const Vec2f& centre, const Vec2f& extents, const T& element) {
    QuadTree<T>& dest_node = static_cast<QuadTree<T>&>(getDestinationNode(centre, extents));
    dest_node._element_infos.emplace_back(centre, extents);
    dest_node._elements.emplace_back(element);
  }

  void clear() {
    _element_infos.clear();
    _elements.clear();
    for (std::unique_ptr<BaseQuadTree>& node : _children) {
      if (nullptr != node) {
        static_cast<QuadTree<T>&>(*node).clear();
      }
    }
  }

  bool pointSweep(const Vec2f& point, const std::function<void(const T&)>& fn) const {
    bool hit = false;

    if (AABB::intersectsPoint(point, _centre, Vec2f(_size))) {
      for (size_t i = 0; (i < _element_infos.size()) && (i < _elements.size()); ++i) {
        const ElementInfo& element_info = _element_infos[i];
        if (AABB::intersectsPoint(point, element_info.centre, element_info.extents)) {
          fn(_elements[i]);
          hit = true;
        }
      }
    }

    for (const auto& child : _children) {
      if (nullptr != child) {
        hit = hit || static_cast<const QuadTree<T>*>(child.get())->pointSweep(point, fn);
      }
    }

    return hit;
  }

  bool aabbCast(const Vec2f& box_centre,
                const Vec2f& box_extents,
                const Vec2f& dir,
                float max_length,
                Hit& hit) const {
    hit = Hit();
    return BaseQuadTree::aabbCast(box_centre, box_extents, dir, max_length, hit);
  }

  bool rayCast(const Vec2f& start, const Vec2f& dir, float max_length, Hit& hit) const {
    hit = Hit();
    return BaseQuadTree::rayCast(start, dir.normalised(), max_length, hit);
  }

 protected:
  virtual std::unique_ptr<BaseQuadTree> moveToNewNode() override {
    std::unique_ptr<QuadTree<T>> node = std::make_unique<QuadTree<T>>(_centre, _size, _min_size);
    std::swap(_children, node->_children);
    _elements.swap(node->_elements);
    return node;
  }

  virtual std::unique_ptr<BaseQuadTree> createNode(const Vec2f& centre, float size) override {
    return std::make_unique<QuadTree<T>>(centre, size, _min_size);
  }

  virtual void setElement(BaseHit& hit, size_t i) const override {
    static_cast<Hit&>(hit).element = _elements[i];
  }

  std::vector<T> _elements;
};
}  // namespace pancake