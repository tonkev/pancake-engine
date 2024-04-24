#pragma once

#include "graphics/vertex.hpp"
#include "resources/resource.hpp"

#include <span>

namespace pancake {
class MeshResourceInterface {
 public:
  MeshResourceInterface() = default;
  virtual ~MeshResourceInterface() = default;

  virtual std::span<const Vertex> getVertices() const = 0;
  virtual std::span<const unsigned int> getIndices() const = 0;

  virtual Resource& asResource() = 0;
};
}  // namespace pancake