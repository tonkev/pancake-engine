#include "graphics/texture.hpp"

#include "resources/image_resource.hpp"
#include "resources/resources.hpp"
#include "resources/texture_props_resource.hpp"

#include <limits>

using namespace pancake;

Texture::Texture(const TexturePropsResource& texture_props)
    : _guid(texture_props.guid()),
      _gen(std::numeric_limits<uint64_t>::max()),
      _image_guid(texture_props.getSourceImage()),
      _image_gen(std::numeric_limits<uint64_t>::max()) {}

const Vec4f& Texture::transform() const {
  const static Vec4f transform(0.f, 0.f, 1.f, 1.f);
  return transform;
}

void Texture::update(Resources& resources) {
  if (const auto texture_props_opt = resources.getOrCreate<TexturePropsResource>(_guid);
      texture_props_opt.has_value()) {
    const TexturePropsResource& texture_props = texture_props_opt.value();
    if (texture_props.gen() != _gen) {
      update(texture_props);
      _gen = texture_props.gen();

      if (texture_props.getSourceImage() != _image_guid) {
        _image_guid = texture_props.getSourceImage();
        _gen = std::numeric_limits<uint64_t>::max();
      }
    }
  }

  if (const auto image_opt = resources.getOrCreate<ImageResource>(_image_guid);
      image_opt.has_value()) {
    const ImageResource& image = image_opt.value();
    if (image.gen() != _image_gen) {
      update(image);
      _image_gen = image.gen();
    }
  }
}

void Texture::update(const TexturePropsResource& texture_props) {}

void Texture::update(const ImageResource& image) {}

void Texture::scheduleImageUpdate() {
  _gen = std::numeric_limits<uint64_t>::max();
}

const GUID& Texture::guid() const {
  return _guid;
}