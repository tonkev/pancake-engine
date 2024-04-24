#pragma once

#include "graphics/common.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"

namespace pancake {
class Renderer;
class Resources;
class Texture;
class Tileset {
 public:
  Tileset(const GUID& guid);

  void update(Resources& resources);

  Vec4f genTransform(int tile, const Vec4f& start = Vec4f(0.f, 0.f, 1.f, 1.f)) const;

  const GUID& getTextureProps() const;

 private:
  GUID _guid;
  uint64_t _gen;

  GUID _texture_props;

  Vec2i _cell_size;
  Vec2i _grid_size;
  Vec2i _texture_size;
};
}  // namespace pancake