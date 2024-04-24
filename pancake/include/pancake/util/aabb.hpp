#pragma once

#include "util/matrix.hpp"

namespace pancake {

class AABB {
 public:
  AABB() = delete;

  static bool intersects(const Vec2f& centre_a,
                         const Vec2f& extents_a,
                         const Vec2f& centre_b,
                         const Vec2f& extents_b,
                         Vec2f& overlap);

  static bool intersectsRay(const Vec2f& start,
                            const Vec2f& dir,
                            const Vec2f& centre,
                            const Vec2f& extents,
                            Vec2f& normal,
                            float& t);

  static bool intersectsPoint(const Vec2f& point, const Vec2f& centre, const Vec2f& extents);
};
}  // namespace pancake