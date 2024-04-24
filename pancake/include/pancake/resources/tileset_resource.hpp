#pragma once

#include "resources/json_resource.hpp"

#include "util/matrix.hpp"

namespace pancake {
class TilesetResource : public JSONResource {
 public:
  TilesetResource(std::string_view path, const GUID& guid);
  virtual ~TilesetResource() = default;

  void setCellSize(const Vec2i& cell_size);
  void setGridSize(const Vec2i& grid_size);
  void setTextureProps(const GUID& texture_props);

  const Vec2i& getCellSize() const;
  const Vec2i& getGridSize() const;
  const GUID& getTextureProps() const;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  Vec2i _cell_size = Vec2i::ones();
  Vec2i _grid_size = Vec2i::ones();
  GUID _texture_props = GUID::null;
};
}  // namespace pancake