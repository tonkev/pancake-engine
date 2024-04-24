#include "resources/ldtk_resource.hpp"

#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/fewi.hpp"

using namespace pancake;

Resources::Creator<LDtkResource> ldtk_resource_creator;

const Resource::Type LDtkResource::TYPE = Type::LDtk;

LDtkResource::LDtkResource(std::string_view path, const GUID& guid) : JSONResource(path, guid) {}

template <typename T, bool (*ParseFunc)(T&, const JSONObject&)>
void parseArrayOfObjects(std::vector<T>& vector,
                         std::string_view name,
                         const JSONObject& parent_json) {
  if (const auto* array = parent_json.get<JSONArray>(name); (nullptr != array)) {
    vector.reserve(array->size());
    for (size_t i = 0; i < array->size(); ++i) {
      if (const auto* object = (*array)[i].asObject(); (nullptr != object)) {
        if (!ParseFunc(vector.emplace_back(), *object)) {
          vector.erase(std::prev(vector.end()));
        }
      }
    }
  }
}

bool parseInt(int& dest, std::string_view name, const JSONObject& parent_json) {
  if (const auto* num = parent_json.get<JSONNumber>(name); (nullptr != num)) {
    dest = static_cast<int>(num->getInt64());
    return true;
  }
  return false;
}

bool parseFloat(float& dest, std::string_view name, const JSONObject& parent_json) {
  if (const auto* num = parent_json.get<JSONNumber>(name); (nullptr != num)) {
    dest = static_cast<float>(num->getDouble());
    return true;
  }
  return false;
}

bool parseString(std::string& dest, std::string_view name, const JSONObject& parent_json) {
  if (const auto* str = parent_json.get<JSONString>(name); (nullptr != str)) {
    dest = *str;
    return true;
  }
  return false;
}

bool parseVec2i(Vec2i& dest, std::string_view name, const JSONObject& parent_json) {
  if (const auto* vec = parent_json.get<JSONArray>(name); (nullptr != vec) && (2 <= vec->size())) {
    if (const auto* x = (*vec)[0].asNumber(); (nullptr != x)) {
      dest.x() = static_cast<int>(x->getInt64());
      if (const auto* y = (*vec)[1].asNumber(); (nullptr != y)) {
        dest.y() = static_cast<int>(y->getInt64());
        return true;
      }
    }
  }
  return false;
}

void LDtkResource::_load() {
  JSONResource::_load();

  if (const auto* defs = _json.get<JSONObject>("defs"); (nullptr != defs)) {
    parseDefinitions(_defs, *defs);
  }

  parseArrayOfObjects<World, parseWorld>(_worlds, "worlds", _json);
  if (_worlds.empty() && (!parseWorld(_worlds.emplace_back(), _json))) {
    _worlds.erase(std::prev(_worlds.end()));
  }
}

Resource::Type LDtkResource::type() const {
  return Type::LDtk;
}

bool LDtkResource::parseDefinitions(Definitions& definitions, const JSONObject& json) {
  parseArrayOfObjects<TilesetDefinition, parseTilesetDefinition>(definitions.tilesets, "tilesets",
                                                                 json);
  return true;
}

bool LDtkResource::parseTilesetDefinition(TilesetDefinition& tileset_definition,
                                          const JSONObject& json) {
  parseString(tileset_definition.rel_path, "relPath", json);
  parseInt(tileset_definition.tile_grid_size, "tileGridSize", json);
  parseInt(tileset_definition.uid, "uid", json);
  return true;
}

bool LDtkResource::parseWorld(World& world, const JSONObject& json) {
  parseArrayOfObjects<Level, parseLevel>(world.levels, "levels", json);
  return true;
}

bool LDtkResource::parseLevel(Level& level, const JSONObject& json) {
  parseArrayOfObjects<LayerInstance, parseLayerInstance>(level.layer_instances, "layerInstances",
                                                         json);
  parseInt(level.world_xy.x(), "worldX", json);
  parseInt(level.world_xy.y(), "worldY", json);
  return true;
}

bool LDtkResource::parseLayerInstance(LayerInstance& layer_instance, const JSONObject& json) {
  parseInt(layer_instance._grid_size, "__gridSize", json);
  parseInt(layer_instance._px_total_offset_xy.x(), "__pxTotalOffsetX", json);
  parseInt(layer_instance._px_total_offset_xy.y(), "__pxTotalOffsetY", json);
  parseInt(layer_instance._tileset_def_uid, "__tilesetDefUid", json);

  std::string type_str;
  if (parseString(type_str, "__type", json)) {
    if ("IntGrid" == type_str) {
      layer_instance._type = LayerInstance::Type::IntGrid;
    } else if ("Entities" == type_str) {
      layer_instance._type = LayerInstance::Type::Entities;
    } else if ("Tiles" == type_str) {
      layer_instance._type = LayerInstance::Type::Tiles;
    } else if ("AutoLayer" == type_str) {
      layer_instance._type = LayerInstance::Type::AutoLayer;
    }
  }

  parseArrayOfObjects<EntityInstance, parseEntityInstance>(layer_instance.entity_instances,
                                                           "entityInstances", json);
  parseArrayOfObjects<TileInstance, parseTileInstance>(layer_instance.grid_tiles, "gridTiles",
                                                       json);

  return true;
}

bool LDtkResource::parseEntityInstance(EntityInstance& entity_instance, const JSONObject& json) {
  parseString(entity_instance._identifier, "__identifier", json);

  parseVec2i(entity_instance.px, "px", json);
  return true;
}

bool LDtkResource::parseTileInstance(TileInstance& tile_instance, const JSONObject& json) {
  parseFloat(tile_instance.a, "a", json);
  parseInt(tile_instance.f, "f", json);
  parseVec2i(tile_instance.px, "px", json);
  parseVec2i(tile_instance.src, "src", json);
  parseInt(tile_instance.t, "t", json);
  return true;
}

const LDtkResource::Definitions& LDtkResource::constDefs() const {
  return _defs;
}

const std::vector<LDtkResource::World>& LDtkResource::constWorlds() const {
  return _worlds;
}