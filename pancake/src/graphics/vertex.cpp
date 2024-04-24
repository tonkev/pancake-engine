#include "graphics/vertex.hpp"

using namespace pancake;

bool Vertex::operator<(const Vertex& rhs) const {
  return std::tie(position, uv) < std::tie(rhs.position, rhs.uv);
}

bool Vertex::operator==(const Vertex& rhs) const {
  return std::tie(position, uv) == std::tie(rhs.position, rhs.uv);
}