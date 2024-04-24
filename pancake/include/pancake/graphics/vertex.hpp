#pragma once

#include "util/matrix.hpp"

namespace pancake {
struct Vertex {
  Vec4f position = Vec4f(0.f, 0.f, 0.f, 1.f);
  Vec4f normal = Vec4f::zeros();
  Vec4f color = Vec4f::zeros();
  Vec2f uv = Vec2f::zeros();

  bool operator<(const Vertex& rhs) const;
  bool operator==(const Vertex& rhs) const;
};
}  // namespace pancake