#include "util/gltf_importer.hpp"

#include "components/3d.hpp"
#include "ecs/world.hpp"
#include "resources/gltf_primitive_resource.hpp"
#include "resources/gltf_resource.hpp"
#include "resources/image_resource.hpp"
#include "resources/material_resource.hpp"
#include "resources/resources.hpp"
#include "resources/texture_props_resource.hpp"
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

  if ((0 <= node.mesh) && (static_cast<size_t>(node.mesh) < model.meshes.size())) {
    const auto& mesh = model.meshes[node.mesh];
    for (size_t p = 0; p < mesh.primitives.size(); ++p) {
      const auto& primitive = mesh.primitives[p];
      if (const auto gltf_prim_opt = resources.getOrCreate<GltfPrimitiveResource>(
              std::string(gltf.path()) + ":mesh[" + std::to_string(node.mesh) + "]:primitive[" +
                  std::to_string(p) + "]",
              false);
          gltf_prim_opt.has_value()) {
        GltfPrimitiveResource& gltf_prim = gltf_prim_opt.value().get();

        EntityWrapper mesh_entity = entity.createChild();
        mesh_entity.addComponent<Transform3D>();

        gltf_prim.setGltf(gltf.guid());
        gltf_prim.setMesh(node.mesh);
        gltf_prim.setPrimitive(p);

        MeshInstance& mesh_inst = mesh_entity.addComponent<MeshInstance>();
        mesh_inst.mesh = gltf_prim.guid();

        MaterialInstance& mat_inst = mesh_entity.addComponent<MaterialInstance>();
        mat_inst.material = _base_material;

        if ((0 <= primitive.material) &&
            (static_cast<size_t>(primitive.material) < model.materials.size())) {
          const auto base_mat_opt = resources.getOrCreate<MaterialResource>(_base_material);
          const auto mat_opt = resources.getOrCreate<MaterialResource>(
              std::string(gltf.path()) + ":material[" + std::to_string(primitive.material) + "]",
              false);
          if (base_mat_opt.has_value() && mat_opt.has_value()) {
            MaterialResource& base_material_res = base_mat_opt.value();
            MaterialResource& material_res = mat_opt.value();
            const auto& material = model.materials[primitive.material];

            material_res.setShader(base_material_res.getShader());
            material_res.setStage(base_material_res.getStage());
            material_res.setDepthTest(base_material_res.getDepthTest());
            material_res.setLightPassInputName(base_material_res.getLightPassInputName());
            material_res.setViewInputName(base_material_res.getViewInputName());
            for (const ShaderInput& input : base_material_res.getInputs()) {
              material_res.addInput(input);
            }

            const auto& texture_info = material.pbrMetallicRoughness.baseColorTexture;
            if ((0 <= texture_info.index) &&
                (static_cast<size_t>(texture_info.index) < model.textures.size())) {
              if (const auto& texture_props_opt = resources.getOrCreate<TexturePropsResource>(
                      std::string(gltf.path()) + ":textures[" + std::to_string(texture_info.index) +
                          "]",
                      false);
                  texture_props_opt.has_value()) {
                TexturePropsResource& texture_props = texture_props_opt.value();

                const auto& texture = model.textures[texture_info.index];
                if ((0 <= texture.source) &&
                    (static_cast<size_t>(texture.source) < model.images.size())) {
                  if (const auto& image_opt = resources.getOrCreate<ImageResource>(
                          std::string(gltf.dir()) + "/" + model.images[texture.source].uri);
                      image_opt.has_value()) {
                    const ImageResource& image = image_opt.value();
                    texture_props.setSourceImage(image.guid());
                    texture_props.setSize(image.size());
                  }
                }

                material_res.addInput(
                    ShaderInput("base_color_texture", TextureRef(texture_props.guid())));
              }
            }

            mat_inst.material = material_res.guid();
          }
        }
      }
    }
  }

  bool success = true;
  for (int child : node.children) {
    success = importNode(entity.createChild(), resources, gltf, model, child) && success;
  }

  return success;
}