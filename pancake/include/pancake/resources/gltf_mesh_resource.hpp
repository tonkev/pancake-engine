#pragma once

#include "resources/gltf_resource.hpp"
#include "resources/json_resource.hpp"
#include "resources/mesh_resource_interface.hpp"
#include "resources/resource_user.hpp"

#include <vector>

namespace pancake {
struct GltfTag {};

class GltfMeshResource
    : public JSONResource,
      public MeshResourceInterface,
      public ResourceUser<GltfMeshResource,
                          ResourceUse<GltfMeshResource, pancake::GltfResource, GltfTag>> {
 public:
  GltfMeshResource(std::string_view path, const GUID& guid);
  virtual ~GltfMeshResource() = default;

  void setGltf(const GUID& gltf);
  void setId(int id);

  const GUID& getGltf() const;
  int getId() const;

  template <typename T>
  void resourceUpdated(const GltfResource& res);
  void resourcesUpdated();

  virtual void ensureUpdated(Resources& resources) override;

  virtual std::span<const Vertex> getVertices() const override;
  virtual std::span<const unsigned int> getIndices() const override;

  virtual Resource& asResource() override;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  int _id;

  std::vector<Vertex> _vertices;
  std::vector<unsigned int> _indices;
};
}  // namespace pancake