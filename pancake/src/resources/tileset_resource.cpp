#include "resources/tileset_resource.hpp"

#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/jsonify_component.hpp"

using namespace pancake;

Resources::Creator<TilesetResource> tileset_resource_creator;

const Resource::Type TilesetResource::TYPE = Type::Tileset;

TilesetResource::TilesetResource(std::string_view path, const GUID& guid)
    : JSONResource(path, guid) {}

void TilesetResource::_load() {
  JSONResource::_load();
  if (const JSONValue* val = _json.get("cell_size"); nullptr != val) {
    TypeDescLibrary::get<Vec2i>().visit(ComponentifyJSON(&_cell_size, *val));
  }
  if (const JSONValue* val = _json.get("grid_size"); nullptr != val) {
    TypeDescLibrary::get<Vec2i>().visit(ComponentifyJSON(&_grid_size, *val));
  }
  if (const JSONValue* val = _json.get("texture_props"); nullptr != val) {
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&_texture_props, *val));
  }
}

void TilesetResource::_save() {
  _json.clear();
  TypeDescLibrary::get<Vec2i>().visit(JSONifyComponent(_json, "cell_size", &_cell_size));
  TypeDescLibrary::get<Vec2i>().visit(JSONifyComponent(_json, "grid_size", &_grid_size));
  TypeDescLibrary::get<GUID>().visit(JSONifyComponent(_json, "texture_props", &_texture_props));
  JSONResource::_save();
}

void TilesetResource::setCellSize(const Vec2i& cell_size) {
  _cell_size = cell_size;
  updated();
}

void TilesetResource::setGridSize(const Vec2i& grid_size) {
  _grid_size = grid_size;
  updated();
}

void TilesetResource::setTextureProps(const GUID& texture_props) {
  _texture_props = texture_props;
  updated();
}

const Vec2i& TilesetResource::getCellSize() const {
  return _cell_size;
}

const Vec2i& TilesetResource::getGridSize() const {
  return _grid_size;
}

const GUID& TilesetResource::getTextureProps() const {
  return _texture_props;
}

Resource::Type TilesetResource::type() const {
  return Type::Tileset;
}