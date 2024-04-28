#include "util/gltf_importer.hpp"

#include "components/3d.hpp"
#include "ecs/world.hpp"
#include "resources/gltf_mesh_resource.hpp"
#include "resources/gltf_resource.hpp"
#include "resources/material_resource.hpp"
#include "resources/resources.hpp"
#include "util/fewi.hpp"

#include "tinygltf/tiny_gltf.h"

using namespace pancake;

GltfImporter::GltfImporter(const GUID& base_material) : _base_material(base_material) {}

void GltfImporter::import(World& world, Resources& resources, const GltfResource& gltf) const {
  const auto& model = gltf.getModel();
  if (model.scenes.empty()) {
    return;
  }

  std::string base_err_msg("Failed to import gltf : ");
  base_err_msg += gltf.path();

  const auto& scene = model.scenes.front();
  for (const int node_idx : scene.nodes) {
    if (!importNode(world.createEntity(), resources, gltf, model, node_idx)) {
      FEWI::error() << base_err_msg;
    }
  }
}

bool GltfImporter::importNode(const EntityWrapper& entity,
                              Resources& resources,
                              const GltfResource& gltf,
                              const tinygltf::Model& model,
                              int node_idx) const {
  if (model.nodes.size() <= static_cast<size_t>(node_idx)) {
    FEWI::error() << "Missing node " << node_idx << "!";
    return false;
  }

  const auto& node = model.nodes[node_idx];

  Transform3D& transform = entity.addComponent<Transform3D>();
  {
    auto modifier = transform.modify();
    if (3 <= node.translation.size()) {
      modifier.localTranslation() =
          Vec3f(static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]),
                static_cast<float>(node.translation[2]));
    }
    if (4 <= node.rotation.size()) {
      modifier.localRotation() =
          QuaternionF(static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]),
                      static_cast<float>(node.rotation[2]), static_cast<float>(node.rotation[3]));
    }
    if (3 <= node.scale.size()) {
      modifier.localScale() =
          Vec3f(static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]),
                static_cast<float>(node.scale[2]));
    }
  }

  if (0 <= node.mesh) {
    if (const auto gltf_mesh_opt = resources.getOrCreate<GltfMeshResource>(
            std::string(gltf.path()) + ":mesh[" + std::to_string(node.mesh) + "]", false);
        gltf_mesh_opt.has_value()) {
      GltfMeshResource& gltf_mesh = gltf_mesh_opt.value().get();

      gltf_mesh.setGltf(gltf.guid());
      gltf_mesh.setId(node.mesh);

      entity.addComponent<MeshInstance>().mesh = gltf_mesh.guid();
      entity.addComponent<MaterialInstance>().material = _base_material;
    }
  }

  bool success = true;
  for (int child : node.children) {
    success = importNode(entity.createChild(), resources, gltf, model, child) && success;
  }

  return success;
}