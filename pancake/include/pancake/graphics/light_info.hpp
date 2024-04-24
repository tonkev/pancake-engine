#pragma once

#include "util/matrix.hpp"

namespace pancake {
struct PointLight;
struct Transform3D;

struct LightInfo {
  LightInfo() = default;
  LightInfo(const Transform3D& transform, const PointLight& point_light);

  bool operator<(const LightInfo& rhs) const;
  bool operator==(const LightInfo& rhs) const;

  Vec3f position = Vec3f::zeros();
  Vec4f color = Vec4f::ones();
};
}  // namespace pancake