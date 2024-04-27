#pragma once

#include "ecs/world.hpp"
#include "util/guid.hpp"

namespace tinygltf {
class Model;
}

namespace pancake {
class GltfResource;
class Resources;
class GltfImporter {
 public:
  GltfImporter(const GUID& base_material);
  ~GltfImporter() = default;

  void import(World& world, Resources& resources, const GltfResource& gltf) const;

 private:
  bool importNode(const EntityWrapper& entity,
                  Resources& resources,
                  const GltfResource& gltf,
                  const tinygltf::Model& model,
                  int node_idx) const;

  GUID _base_material;
};
}  // namespace pancake