#pragma once

#include "graphics/buffer_format.hpp"
#include "graphics/texture_filter.hpp"
#include "resources/json_resource.hpp"

#include "util/matrix.hpp"

namespace pancake {
class TexturePropsResource : public JSONResource {
 public:
  TexturePropsResource(std::string_view path, const GUID& guid);
  virtual ~TexturePropsResource() = default;

  void setFilter(TextureFilter filter);
  void setFormat(BufferFormat format);
  void setSize(const Vec2i& size);
  void setSourceImage(const GUID& source_image);

  TextureFilter getFilter() const;
  BufferFormat getFormat() const;
  const Vec2i& getSize() const;
  const GUID& getSourceImage() const;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  TextureFilter _filter = TextureFilter::Linear;
  BufferFormat _format = BufferFormat::RGBA32F;
  Vec2i _size = Vec2i::ones();
  GUID _source_image = GUID::null;
};
}  // namespace pancake