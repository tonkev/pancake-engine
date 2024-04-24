#include "graphics/light_info.hpp"

#include "components/3d.hpp"
#include "components/graphics.hpp"

using namespace pancake;

LightInfo::LightInfo(const Transform3D& transform, const PointLight& point_light)
    : position(transform.translation()), color(point_light.color) {}

bool LightInfo::operator<(const LightInfo& rhs) const {
  return std::tie(position, color) < std::tie(rhs.position, rhs.color);
}

bool LightInfo::operator==(const LightInfo& rhs) const {
  return std::tie(position, color) == std::tie(rhs.position, rhs.color);
}