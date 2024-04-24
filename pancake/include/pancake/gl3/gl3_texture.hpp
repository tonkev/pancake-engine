#pragma once

#include "graphics/texture.hpp"

#include <span>

namespace pancake {
class GL3Texture : public Texture {
 public:
  GL3Texture(const TexturePropsResource& texture_props);
  virtual ~GL3Texture();

  virtual void bind(int slot) const override;

  unsigned int getId() const;

  virtual void update(const TexturePropsResource& texture_props) override;
  virtual void update(const ImageResource& image) override;

 private:
  unsigned int _tex;
};
}  // namespace pancake