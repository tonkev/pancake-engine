#pragma once

#include "ecs/world.hpp"

namespace tinygltf {
class Model;
}

namespace pancake {
class GltfResource;
class Resources;
class GltfImporter {
 public:
  GltfImporter();
  ~GltfImporter() = default;

  void import(World& world, Resources& resources, const GltfResource& gltf) const;

 private:
  bool importNode(const EntityWrapper& entity,
                  Resources& resources,
                  const GltfResource& gltf,
                  const tinygltf::Model& model,
                  int node_idx) const;
};
}  // namespace pancake