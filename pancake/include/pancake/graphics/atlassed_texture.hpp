#pragma once

#include "graphics/texture.hpp"

namespace pancake {
class ImageAtlas;
class GL3Renderer;
class AtlassedTexture : public Texture {
 public:
  virtual ~AtlassedTexture() = default;

  virtual void bind(int slot) const override;

  virtual const Vec4f& transform() const override;

  Texture& atlas() const;

 protected:
  AtlassedTexture(Ptr<Texture> texture,
                  Ptr<ImageAtlas> atlas,
                  const TexturePropsResource& texture_props);

  virtual void update(Resources& resources) override;

  friend GL3Renderer;

 private:
  Ptr<Texture> _texture;
  Ptr<ImageAtlas> _atlas;
  Vec4f _transform;
};
}  // namespace pancake