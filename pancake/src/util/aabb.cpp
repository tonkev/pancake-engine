#include "util/aabb.hpp"

using namespace pancake;

bool AABB::intersects(const Vec2f& centre_a,
                      const Vec2f& extents_a,
                      const Vec2f& centre_b,
                      const Vec2f& extents_b,
                      Vec2f& overlap) {
  const float left_a = centre_a.x() - extents_a.x();
  const float right_a = centre_a.x() + extents_a.x();
  const float top_a = centre_a.y() + extents_a.y();
  const float bottom_a = centre_a.y() - extents_a.y();

  const float left_b = centre_b.x() - extents_b.x();
  const float right_b = centre_b.x() + extents_b.x();
  const float top_b = centre_b.y() + extents_b.y();
  const float bottom_b = centre_b.y() - extents_b.y();

  if (left_a < left_b) {
    overlap.x() = right_a - left_b;
  } else {
    overlap.x() = right_b - left_a;
  }

  if (bottom_a < bottom_b) {
    overlap.y() = top_a - bottom_b;
  } else {
    overlap.y() = top_b - bottom_a;
  }

  overlap = overlap.min(extents_a * 2.f);
  overlap = overlap.min(extents_b * 2.f);

  return (0.0001f < overlap.x()) && (0.0001f < overlap.y());
}

bool AABB::intersectsRay(const Vec2f& start,
                         const Vec2f& dir,
                         const Vec2f& centre,
                         const Vec2f& extents,
                         Vec2f& normal,
                         float& t) {
  const Vec2f dir_sign = dir.sign();
  Vec2f closest = centre - extents.mask(dir_sign);

  Vec2f gradients;
  gradients.x() =
      (0.0001f < std::abs(dir.y())) ? (dir.x() / dir.y()) : std::numeric_limits<float>::infinity();
  gradients.y() =
      (0.0001f < std::abs(dir.x())) ? (dir.y() / dir.x()) : std::numeric_limits<float>::infinity();

  Vec2f y_hit = Vec2f(start.x() + ((closest.y() - start.y()) * gradients.x()), closest.y());
  Vec2f x_hit = Vec2f(closest.x(), start.y() + ((closest.x() - start.x()) * gradients.y()));

  t = std::numeric_limits<float>::infinity();
  if ((centre.x() - extents.x() <= y_hit.x()) && (y_hit.x() <= centre.x() + extents.x())) {
    float y_t = (y_hit - start).dot(dir);
    if (0.f <= y_t) {
      t = y_t;
      normal = Vec2f(0.f, -dir_sign.y());
    }
  }

  if ((centre.y() - extents.y() <= x_hit.y()) && (x_hit.y() <= centre.y() + extents.y())) {
    float x_t = (x_hit - start).dot(dir);
    if ((0.f <= x_t) && (x_t < t)) {
      t = x_t;
      normal = Vec2f(-dir_sign.x(), 0.f);
    }
  }

  return std::isfinite(t) && (0.f <= t);
}

bool AABB::intersectsPoint(const Vec2f& point, const Vec2f& centre, const Vec2f& extents) {
  return ((centre.x() - extents.x() <= point.x()) && (point.x() <= centre.x() + extents.x())) &&
         ((centre.y() - extents.y() <= point.y()) && (point.y() <= centre.y() + extents.y()));
}