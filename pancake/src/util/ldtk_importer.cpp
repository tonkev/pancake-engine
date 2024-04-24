#include "util/ldtk_importer.hpp"

#include "components/2d.hpp"
#include "ecs/world.hpp"
#include "resources/image_resource.hpp"
#include "resources/ldtk_resource.hpp"
#include "resources/resources.hpp"
#include "resources/texture_props_resource.hpp"
#include "resources/tileset_resource.hpp"

#include <filesystem>

using namespace pancake;
using LDtk = LDtkResource;

void LDtkImporter::GlobalEntityInstanceRule::operator()(
    World& world,
    const Entity& layer_instance_ent,
    const LDtkResource::EntityInstance& entity_instance) const {
  const EntityWrapper entity = world.createEntity();

  {
    Transform2D::Accessor transform = entity.addComponent<Transform2D>().modify();
    if (world.hasComponent<Transform2D>(layer_instance_ent)) {
      transform.translation() += world.getComponent<Transform2D>(layer_instance_ent).translation();
    }
    transform.translation() += Vec2f(static_cast<float>(entity_instance.px.x()),
                                     static_cast<float>(-entity_instance.px.y()));
  }

  operator()(entity, entity_instance);
}

class DefaultTileRule : public LDtkImporter::TileRule {
 public:
  virtual void operator()(const EntityWrapper& entity,
                          const LDtkResource::TileInstance& tile_instance,
                          const LDtkResource::TilesetDefinition& tileset_def,
                          const GUID& tileset_guid) const override {
    {
      Transform2D::Accessor transform = entity.addComponent<Transform2D>().modify();
      transform.translation() = Vec2i(tile_instance.px.x(), -tile_instance.px.y());
      transform.scale() = Vec2i::ones() * tileset_def.tile_grid_size;
    }

    Sprite2D& sprite = entity.addComponent<Sprite2D>();
    sprite.texture.texture = tileset_guid;
    sprite.texture.tile = tile_instance.t;

    entity.addComponent<StaticBody2D>();
    entity.addComponent<RectangleCollider2D>();
  }
};

void LDtkImporter::IntermediaryInfo::addTilesetInfo(
    const LDtkResource::TilesetDefinition& tileset_def,
    const GUID& guid) {
  _tileset_infos.try_emplace(tileset_def.uid, tileset_def, guid);
}

const LDtkImporter::IntermediaryInfo::TilesetInfo* LDtkImporter::IntermediaryInfo::getTilesetInfo(
    int uid) const {
  if (const auto it = _tileset_infos.find(uid); it != _tileset_infos.end()) {
    return &(it->second);
  }
  return nullptr;
}

LDtkImporter::LDtkImporter() : _tile_rule(std::make_unique<DefaultTileRule>()) {}

void LDtkImporter::addEntityInstanceRule(std::string_view identifier,
                                         std::unique_ptr<EntityInstanceRule> entity_instance_rule) {
  _entity_instance_rules.emplace(identifier, entity_instance_rule.release());
}

void LDtkImporter::setTileRule(std::unique_ptr<TileRule> tile_rule) {
  if (nullptr != tile_rule) {
    _tile_rule.swap(tile_rule);
  }
}

void LDtkImporter::import(World& world, Resources& resources, const LDtkResource& ldtk) const {
  const LDtk::Definitions& defs = ldtk.constDefs();

  IntermediaryInfo info;
  for (const LDtk::TilesetDefinition& tileset_def : defs.tilesets) {
    importTilesetDefinition(info, resources, tileset_def, std::string(ldtk.path()));
  }

  if (!ldtk.constWorlds().empty()) {
    const LDtk::World& ldtk_world = ldtk.constWorlds().front();

    if (!ldtk_world.levels.empty()) {
      const LDtk::Level& level = ldtk_world.levels.front();

      const EntityWrapper level_entity = world.createEntity();
      Transform2D level_transform;
      level_transform.modify().localTranslation() = Vec2i(level.world_xy.x(), -level.world_xy.y());
      level_entity.addComponent<Transform2D>(level_transform);

      for (const LDtk::LayerInstance& layer_instance : level.layer_instances) {
        const EntityWrapper layer_instance_entity = world.createEntity();
        layer_instance_entity.parentTo(level_entity);
        Transform2D& layer_instance_transform = layer_instance_entity.addComponent<Transform2D>();
        layer_instance_transform.setParentGlobalMatrices(level_transform);
        layer_instance_transform.modify().localTranslation() =
            Vec2i(layer_instance._px_total_offset_xy.x(), -layer_instance._px_total_offset_xy.y());

        if (LDtk::LayerInstance::Type::Entities == layer_instance._type) {
          importEntityLayerInstance(world, layer_instance_entity.entity(), layer_instance);
        } else if (LDtk::LayerInstance::Type::Tiles == layer_instance._type) {
          importTileLayerInstance(world, layer_instance_entity.entity(), layer_instance, info);
        }
      }
    }
  }
}

void LDtkImporter::importTilesetDefinition(IntermediaryInfo& info,
                                           Resources& resources,
                                           const LDtkResource::TilesetDefinition& tileset_def,
                                           const std::string& ldtk_path) const {
  const std::string tileset_virtual_path =
      ldtk_path + ":tileset[" + std::to_string(tileset_def.uid) + "]";
  const auto tileset_opt = resources.getOrCreate<TilesetResource>(tileset_virtual_path, false);
  if (tileset_opt.has_value()) {
    TilesetResource& tileset_res = tileset_opt.value();
    tileset_res.setCellSize(Vec2i(tileset_def.tile_grid_size));

    const std::string texture_props_virtual_path =
        ldtk_path + ":texture_props[" + std::to_string(tileset_def.uid) + "]";
    const auto texture_props_opt =
        resources.getOrCreate<TexturePropsResource>(texture_props_virtual_path, false);
    if (texture_props_opt.has_value()) {
      TexturePropsResource& texture_props_res = texture_props_opt.value();

      const std::string tileset_image_path = std::filesystem::path(ldtk_path)
                                                 .replace_filename(tileset_def.rel_path)
                                                 .lexically_normal()
                                                 .string();
      const auto image_opt = resources.getOrCreate<ImageResource>(tileset_image_path);
      if (image_opt.has_value()) {
        const ImageResource& image_res = image_opt.value();
        tileset_res.setGridSize(image_res.size() / tileset_def.tile_grid_size);

        texture_props_res.setSize(image_res.size());
        texture_props_res.setSourceImage(image_res.guid());
      }

      tileset_res.setTextureProps(texture_props_res.guid());
    }

    info.addTilesetInfo(tileset_def, tileset_res.guid());
  }
}

void LDtkImporter::importEntityLayerInstance(World& world,
                                             const Entity& entity,
                                             const LDtk::LayerInstance& layer_instance) const {
  for (const LDtk::EntityInstance& entity_instance : layer_instance.entity_instances) {
    if (const auto it = _entity_instance_rules.find(entity_instance._identifier);
        (it != _entity_instance_rules.end()) && (nullptr != it->second)) {
      (*it->second)(world, entity, entity_instance);
    }
  }
}

void LDtkImporter::importTileLayerInstance(World& world,
                                           const Entity& entity,
                                           const LDtk::LayerInstance& layer_instance,
                                           const IntermediaryInfo& info) const {
  if (const IntermediaryInfo::TilesetInfo* tileset_info =
          info.getTilesetInfo(layer_instance._tileset_def_uid);
      (nullptr != tileset_info) && (nullptr != _tile_rule)) {
    for (const LDtk::TileInstance& tile_instance : layer_instance.grid_tiles) {
      const EntityWrapper tile_entity = world.createEntity();
      tile_entity.parentTo(entity);
      (*_tile_rule)(tile_entity, tile_instance, tileset_info->def, tileset_info->guid);
    }
  }
}