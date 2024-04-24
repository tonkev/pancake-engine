#pragma once

#include "resources/mesh_resource_interface.hpp"
#include "resources/resource_user.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"

#include "graphics/shader.hpp"

namespace pancake {
struct MeshRes {};

class Mesh : public ResourceUser<Mesh, ResourceUse<Mesh, MeshResourceInterface, MeshRes>> {
 public:
  virtual ~Mesh() = default;

  virtual void update(std::span<const Vertex> vertices, std::span<const unsigned int> indices) = 0;
  virtual void draw(unsigned int num_instances = 1) const = 0;

  template <typename T>
  void resourceUpdated(const MeshResourceInterface& res);

  void resourcesUpdated();

  const GUID& guid() const;

 protected:
  Mesh(const GUID& guid);

 private:
  GUID _guid;
};
}  // namespace pancake