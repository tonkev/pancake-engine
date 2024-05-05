#pragma once

#include "pancake.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"

namespace pancake {
class ImageResource;
class TexturePropsResource;
class Resources;
class Texture {
 public:
  virtual ~Texture() = default;

  virtual void bind(int slot) const = 0;

  virtual void update(Resources& resources);
  virtual void update(const TexturePropsResource& texture_props);
  virtual void update(const ImageResource& image);

  virtual const Vec4f& transform() const;

  const GUID& guid() const;
  const GUID& imageGuid() const;
  virtual const GUID& bindingGuid() const;

 protected:
  Texture(const TexturePropsResource& texture_props);

  void scheduleImageUpdate();

  GUID _guid;
  uint64_t _gen;

  GUID _image_guid;
  uint64_t _image_gen;
};
}  // namespace pancake