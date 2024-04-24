#include "util/quad_tree.hpp"

#include "util/aabb.hpp"

using namespace pancake;

BaseQuadTree::BaseQuadTree(const Vec2f& centre, float size, float min_size)
    : _centre(centre), _size(size), _min_size(min_size) {}

bool BaseQuadTree::aabbCast(const Vec2f& box_centre,
                            const Vec2f& box_extents,
                            const Vec2f& dir,
                            float max_length,
                            BaseHit& hit) const {
  const Vec2f norm_dir = dir.normalised();

  Vec2f gradients;
  gradients.x() = (0.0001f < std::abs(norm_dir.y())) ? (norm_dir.x() / norm_dir.y())
                                                     : std::numeric_limits<float>::infinity();
  gradients.y() = (0.0001f < std::abs(norm_dir.x())) ? (norm_dir.y() / norm_dir.x())
                                                     : std::numeric_limits<float>::infinity();

  Vec2f sweep_offset = norm_dir * max_length * 0.5f;
  return aabbCast(box_centre + sweep_offset, box_extents + sweep_offset.abs(), box_centre,
                  box_extents, norm_dir, gradients, hit);
}

bool BaseQuadTree::aabbCast(const Vec2f& sweep_centre,
                            const Vec2f& sweep_extents,
                            const Vec2f& box_centre,
                            const Vec2f& box_extents,
                            const Vec2f& dir,
                            const Vec2f& gradients,
                            BaseHit& hit) const {
  Vec2f overlap;
  if (AABB::intersects(_centre, Vec2f(_size * 0.5f), sweep_centre, sweep_extents, overlap)) {
    for (size_t i = 0; i < _element_infos.size(); ++i) {
      const ElementInfo& element_info = _element_infos[i];
      Vec2f overlap, exit;
      if (AABB::intersects(element_info.centre, element_info.extents, sweep_centre, sweep_extents,
                           overlap)) {
        Vec2f next_hit =
            element_info.centre - (box_extents + element_info.extents).mask(dir.sign());
        Vec2f step = next_hit - box_centre;

        Vec2f y_hit = Vec2f(box_centre.x() + (step.y() * gradients.x()), next_hit.y());
        Vec2f x_hit = Vec2f(next_hit.x(), box_centre.y() + (step.x() * gradients.y()));

        Vec2f y_hit_rel = y_hit - box_centre;
        Vec2f x_hit_rel = x_hit - box_centre;
        Vec2i dir_sign = dir.sign();

        float next_hit_length = std::numeric_limits<float>::infinity();
        float y_hit_length =
            y_hit_rel.norm() * ((Vec2i(y_hit_rel.sign()) == dir_sign) ? 1.f : -1.f);
        float x_hit_length =
            x_hit_rel.norm() * ((Vec2i(x_hit_rel.sign()) == dir_sign) ? 1.f : -1.f);

        Vec2f normal;
        Vec2f confirm_extents;
        if ((0.f <= x_hit_length) && std::isfinite(x_hit_length) &&
            ((!std::isfinite(y_hit_length)) || (y_hit_length < x_hit_length))) {
          normal = Vec2f(-dir.sign().x(), 0.f);
          next_hit = x_hit;
          next_hit_length = x_hit_length;
          confirm_extents = box_extents.mask(Vec2f(2.f, 1.f));
        } else if ((0.f <= y_hit_length) && (std::isfinite(y_hit_length))) {
          normal = Vec2f(0.f, -dir.sign().y());
          next_hit = y_hit;
          next_hit_length = y_hit_length;
          confirm_extents = box_extents.mask(Vec2f(1.f, 2.f));
        }

        if ((next_hit_length < hit.length) &&
            AABB::intersects(element_info.centre, element_info.extents, next_hit, confirm_extents,
                             overlap)) {
          hit.hit = next_hit;
          hit.normal = normal;
          hit.length = next_hit_length;
          setElement(hit, i);
        }
      }
    }

    for (const std::unique_ptr<BaseQuadTree>& child : _children) {
      if (nullptr != child) {
        child->aabbCast(sweep_centre, sweep_extents, box_centre, box_extents, dir, gradients, hit);
      }
    }
  }

  return std::isfinite(hit.length);
}

bool BaseQuadTree::rayCast(const Vec2f& start,
                           const Vec2f& dir,
                           float max_length,
                           BaseHit& hit) const {
  const Vec2f extents(_size * 0.5f);
  Vec2f normal;
  float t;
  if (AABB::intersectsPoint(start, _centre, extents) ||
      (AABB::intersectsRay(start, dir, _centre, extents, normal, t) && (t <= max_length))) {
    for (size_t i = 0; i < _element_infos.size(); ++i) {
      const ElementInfo& element_info = _element_infos[i];
      if (AABB::intersectsRay(start, dir, element_info.centre, element_info.extents, normal, t) &&
          (t <= max_length) && (t < hit.length)) {
        hit.hit = start + (dir * t);
        hit.normal = normal;
        hit.length = t;
        setElement(hit, i);
      }
    }
  }

  for (const std::unique_ptr<BaseQuadTree>& child : _children) {
    if (nullptr != child) {
      child->rayCast(start, dir, max_length, hit);
    }
  }

  return std::isfinite(hit.length);
}

BaseQuadTree& BaseQuadTree::getDestinationNode(const Vec2f& box_centre, const Vec2f& box_extents) {
  const Vec2f extents(_size * 0.5f);
  Vec2f overlap;
  AABB::intersects(_centre, extents, box_centre, box_extents, overlap);
  if ((1.f <= overlap.x()) && (1.f <= overlap.y())) {
    if (_min_size < (_size * 0.5f)) {
      for (int i = 0; i < 4; ++i) {
        const Vec2f sub_node_extents(_size * 0.25f);
        const Vec2f sub_node_centre =
            _centre +
            sub_node_extents.mask(Vec2f((0 == (i % 2)) ? -1.f : 1.f, (i < 2) ? 1.f : -1.f));

        if (AABB::intersects(sub_node_centre, sub_node_extents, box_centre, box_extents, overlap)) {
          overlap = overlap.mask((box_extents * 2.f).reciprocal());
          if (((overlap.x() < 1.f) || (overlap.y() < 1.f))) {
            break;
          }

          if (nullptr == _children[i]) {
            _children[i] = createNode(sub_node_centre, _size * 0.5f);
          }

          return _children[i]->getDestinationNode(box_centre, box_extents);
        }
      }
    }
  } else {
    const Vec2f dir = (_centre - box_centre).sign();
    int i = ((0.f < dir.x()) ? 1 : 0) + ((0.f < dir.y()) ? 0 : 2);
    _children[i] = moveToNewNode();

    _centre += extents.mask(-dir);
    _size *= 2.f;

    return getDestinationNode(box_centre, box_extents);
  }

  return *this;
}