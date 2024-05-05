#include "graphics/atlassed_texture.hpp"

#include "graphics/image_atlas.hpp"
#include "resources/texture_props_resource.hpp"
#include "util/fewi.hpp"

using namespace pancake;

AtlassedTexture::AtlassedTexture(Ptr<Texture> texture,
                                 Ptr<ImageAtlas> atlas,
                                 const TexturePropsResource& texture_props)
    : Texture(texture_props), _texture(texture), _atlas(atlas) {
  atlas->add(texture_props.getSourceImage());
}

void AtlassedTexture::update(Resources& resources) {
  if (const auto rect_opt = _atlas->getRect(_image_guid); rect_opt.has_value()) {
    const Vec2f& atlas_size = _atlas->size();
    const ImageAtlas::Rect& rect = rect_opt.value();

    _transform.x() = rect.position.x() / atlas_size.x();
    _transform.y() = rect.position.y() / atlas_size.y();
    _transform.z() = rect.size.x() / atlas_size.x();
    _transform.w() = rect.size.y() / atlas_size.y();
  }

  Texture::update(resources);
}

void AtlassedTexture::bind(int slot) const {
  _texture->bind(slot);
}

const Vec4f& AtlassedTexture::transform() const {
  return _transform;
}

Texture& AtlassedTexture::atlas() const {
  return *_texture;
}

void AtlassedTexture::setAtlas(Ptr<Texture> texture, Ptr<ImageAtlas> atlas) {
  _texture = texture;
  _atlas = atlas;
  atlas->add(_image_guid);
}

const GUID& AtlassedTexture::bindingGuid() const {
  return _texture->bindingGuid();
}