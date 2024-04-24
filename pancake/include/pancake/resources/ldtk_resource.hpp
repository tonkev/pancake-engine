#pragma once

#include "resources/json_resource.hpp"

#include "util/matrix.hpp"

#include <vector>

namespace pancake {
class LDtkResource : public JSONResource {
 public:
  struct TilesetDefinition {
    std::string rel_path;
    int tile_grid_size;
    int uid;
  };

  struct Definitions {
    std::vector<TilesetDefinition> tilesets;
  };

  struct EntityInstance {
    std::string _identifier;

    Vec2i px;
  };

  struct TileInstance {
    float a;
    int f;
    Vec2i px;
    Vec2i src;
    int t;
  };

  struct LayerInstance {
    enum class Type { IntGrid, Entities, Tiles, AutoLayer };

    int _grid_size;
    Vec2i _px_total_offset_xy;
    int _tileset_def_uid;
    Type _type;

    std::vector<EntityInstance> entity_instances;
    std::vector<TileInstance> grid_tiles;
  };

  struct Level {
    std::vector<LayerInstance> layer_instances;
    Vec2i world_xy;
  };

  struct World {
    std::vector<Level> levels;
  };

  LDtkResource(std::string_view path, const GUID& guid);
  virtual ~LDtkResource() = default;

  virtual Type type() const override;

  static const Type TYPE;

  const Definitions& constDefs() const;
  const std::vector<World>& constWorlds() const;

 protected:
  virtual void _load() override;

 private:
  static bool parseDefinitions(Definitions& defs, const JSONObject& json);
  static bool parseTilesetDefinition(TilesetDefinition& tileset_definition, const JSONObject& json);

  static bool parseWorld(World& world, const JSONObject& json);
  static bool parseLevel(Level& level, const JSONObject& json);
  static bool parseLayerInstance(LayerInstance& layer_instance, const JSONObject& json);
  static bool parseEntityInstance(EntityInstance& entity_instance, const JSONObject& json);
  static bool parseTileInstance(TileInstance& tile_instance, const JSONObject& json);

  Definitions _defs;
  std::vector<World> _worlds;
};
}  // namespace pancake