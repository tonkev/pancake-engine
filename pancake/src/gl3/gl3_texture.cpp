#include "gl3/gl3_texture.hpp"

#include "resources/image_resource.hpp"
#include "resources/texture_props_resource.hpp"
#include "util/fewi.hpp"

#include "GL/gl3w.h"

using namespace pancake;

GL3Texture::GL3Texture(const TexturePropsResource& texture_props)
    : Texture(texture_props), _tex(0) {
  glGenTextures(1, &_tex);
  update(texture_props);
}

GL3Texture::~GL3Texture() {
  glDeleteTextures(1, &_tex);
}

void GL3Texture::bind(int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, _tex);
}

unsigned int GL3Texture::getId() const {
  return _tex;
}

void GL3Texture::update(const TexturePropsResource& texture_props) {
  Texture::update(texture_props);

  const Vec2i& size = texture_props.getSize();

  glBindTexture(GL_TEXTURE_2D, _tex);

  GLenum err = GL_NO_ERROR;
  switch (texture_props.getFormat()) {
    case BufferFormat::RGBA32F:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   nullptr);
      err = glGetError();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    case BufferFormat::RGBA32UI:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, size.x(), size.y(), 0, GL_RGBA_INTEGER,
                   GL_UNSIGNED_INT, nullptr);
      err = glGetError();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
    default:
      FEWI::error() << "Unsupported buffer format!";
      break;
  }

  if (err != GL_NO_ERROR) {
    FEWI::error() << "Failed to create texture! (" << err << ")";
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  scheduleImageUpdate();
}

void GL3Texture::update(const ImageResource& image) {
  Texture::update(image);

  unsigned int channels = image.channels();
  glBindTexture(GL_TEXTURE_2D, _tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(),
                  channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image.data());
  glBindTexture(GL_TEXTURE_2D, 0);
}