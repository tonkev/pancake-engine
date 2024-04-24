#pragma once

#include "graphics/framebuffer.hpp"

#include "gl3/gl3_texture.hpp"
#include "resources/texture_props_resource.hpp"
#include "util/matrix.hpp"

namespace pancake {
class GL3Framebuffer : public Framebuffer {
 public:
  GL3Framebuffer(const GUID& guid, const FramebufferInfo& info);
  virtual ~GL3Framebuffer();

  unsigned int id();

  virtual void bind() override;
  virtual void preRender() override;

  void blit(const GL3Framebuffer& src);

  virtual OptTextureConstRef texture(size_t i = 0) const override;
  virtual void readPixel(size_t attachment, const Vec2u& position, Vec4u& pixel) const override;

 protected:
  virtual void update() override;

 private:
  std::vector<Ptr<GL3Texture>> _render_targets{};

  unsigned int _fbo = 0;
  unsigned int _depth_stencil = 0;
};
}  // namespace pancake