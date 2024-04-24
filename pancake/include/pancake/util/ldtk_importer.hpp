#pragma once

#include "ecs/world.hpp"
#include "resources/ldtk_resource.hpp"

#include <memory>
#include <unordered_map>

namespace pancake {
class Resources;
class TilesetResource;
class LDtkImporter {
 public:
  class EntityInstanceRule {
   public:
    virtual void operator()(World& world,
                            const Entity& layer_instance_ent,
                            const LDtkResource::EntityInstance& entity_instance) const = 0;

    virtual void operator()(const EntityWrapper& entity,
                            const LDtkResource::EntityInstance& entity_instance) const = 0;
  };

  class GlobalEntityInstanceRule : public EntityInstanceRule {
   public:
    virtual void operator()(World& world,
                            const Entity& layer_instance_ent,
                            const LDtkResource::EntityInstance& entity_instance) const override;

    virtual void operator()(const EntityWrapper& entity,
                            const LDtkResource::EntityInstance& entity_instance) const = 0;
  };

  class TileRule {
   public:
    virtual void operator()(const EntityWrapper& entity,
                            const LDtkResource::TileInstance& tile_instance,
                            const LDtkResource::TilesetDefinition& tileset_def,
                            const GUID& tileset_guid) const = 0;
  };

  LDtkImporter();
  ~LDtkImporter() = default;

  void addEntityInstanceRule(std::string_view identifier,
                             std::unique_ptr<EntityInstanceRule> entity_instance_rule);

  void setTileRule(std::unique_ptr<TileRule> tile_rule);

  void import(World& world, Resources& resources, const LDtkResource& ldtk) const;

 private:
  class IntermediaryInfo {
   public:
    struct TilesetInfo {
      const LDtkResource::TilesetDefinition& def;
      GUID guid;
    };

    void addTilesetInfo(const LDtkResource::TilesetDefinition& tileset_def, const GUID& guid);

    const TilesetInfo* getTilesetInfo(int uid) const;

   private:
    std::unordered_map<int, TilesetInfo> _tileset_infos;
  };

  void importTilesetDefinition(IntermediaryInfo& info,
                               Resources& resources,
                               const LDtkResource::TilesetDefinition& tileset_def,
                               const std::string& ldtk_path) const;

  void importEntityLayerInstance(World& world,
                                 const Entity& entity,
                                 const LDtkResource::LayerInstance& layer_instance) const;
  void importTileLayerInstance(World& world,
                               const Entity& entity,
                               const LDtkResource::LayerInstance& layer_instance,
                               const IntermediaryInfo& info) const;

  std::unordered_map<std::string, std::unique_ptr<EntityInstanceRule>> _entity_instance_rules;
  std::unique_ptr<TileRule> _tile_rule;
};
}  // namespace pancake