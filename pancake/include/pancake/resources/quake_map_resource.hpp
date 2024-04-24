#pragma once

#include "resources/mesh_resource_interface.hpp"
#include "resources/text_resource.hpp"

#include "util/quake_map.hpp"

#include <vector>

namespace pancake {
class QuakeMapResource : public TextResource, public MeshResourceInterface {
 public:
  QuakeMapResource(std::string_view path, const GUID& guid);
  virtual ~QuakeMapResource() = default;

  virtual std::span<const Vertex> getVertices() const override;
  virtual std::span<const unsigned int> getIndices() const override;

  virtual Resource& asResource() override;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;

 private:
  QuakeMap _map;
  std::vector<Vertex> _vertices;
  std::vector<unsigned int> _indices;
};
}  // namespace pancake