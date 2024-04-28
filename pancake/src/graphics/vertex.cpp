#include "graphics/vertex.hpp"

using namespace pancake;

bool Vertex::operator<(const Vertex& rhs) const {
  return std::tie(position, normal, tangent, color, uv0, uv1) <
         std::tie(rhs.position, rhs.normal, rhs.tangent, rhs.color, rhs.uv0, rhs.uv1);
}

bool Vertex::operator==(const Vertex& rhs) const {
  return std::tie(position, normal, tangent, color, uv0, uv1) ==
         std::tie(rhs.position, rhs.normal, rhs.tangent, rhs.color, rhs.uv0, rhs.uv1);
}