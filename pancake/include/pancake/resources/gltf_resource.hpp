#pragma once

#include "resources/resource.hpp"

#include <memory>

namespace tinygltf {
class Model;
}

namespace pancake {
class GltfImporter;
class GltfMeshResource;
class GltfResource : public Resource {
 public:
  GltfResource(std::string_view path, const GUID& guid);
  virtual ~GltfResource() = default;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

  const tinygltf::Model& getModel() const;

  friend GltfImporter;
  friend GltfMeshResource;

 private:
  std::unique_ptr<tinygltf::Model> _model;
};
}  // namespace pancake