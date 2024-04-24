#include "graphics/tileset.hpp"

#include "core/renderer.hpp"
#include "resources/resources.hpp"
#include "resources/texture_props_resource.hpp"
#include "resources/tileset_resource.hpp"

#include <limits>

using namespace pancake;

Tileset::Tileset(const GUID& guid)
    : _guid(guid),
      _gen(std::numeric_limits<uint64_t>::max()),
      _cell_size(Vec2i::ones()),
      _grid_size(Vec2i::ones()){};

void Tileset::update(Resources& resources) {
  if (const auto res_opt = resources.getOrCreate<TilesetResource>(_guid); res_opt.has_value()) {
    const TilesetResource& res = res_opt.value();
    const auto res_gen = res.gen();
    if (res_gen != _gen) {
      _gen = res_gen;
      _cell_size = res.getCellSize().max(Vec2i::ones());
      _grid_size = res.getGridSize().max(Vec2i::ones());

      const auto res_texture_props = res.getTextureProps();
      _texture_props = res_texture_props;
    }
  }

  if (const auto texture_props_opt = resources.getOrCreate<TexturePropsResource>(_texture_props);
      texture_props_opt.has_value()) {
    _texture_size = texture_props_opt.value().get().getSize().max(Vec2i::ones());
  }
}

Vec4f Tileset::genTransform(int tile, const Vec4f& start) const {
  Vec4f transform = start;

  Vec2i texture_size = Vec2i::ones();
  Vec2i tile_coords = Vec2i(tile % _grid_size.x(), tile / _grid_size.x());

  transform.z() *= _cell_size.x() / static_cast<float>(_texture_size.x());
  transform.w() *= _cell_size.y() / static_cast<float>(_texture_size.y());

  transform.x() += tile_coords.x() * transform.z();
  transform.y() += tile_coords.y() * transform.w();

  return transform;
}

const GUID& Tileset::getTextureProps() const {
  return _texture_props;
}