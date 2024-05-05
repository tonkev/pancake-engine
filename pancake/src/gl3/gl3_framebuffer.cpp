#include "gl3/gl3_framebuffer.hpp"

#include "util/fewi.hpp"

#include "GL/gl3w.h"

using namespace pancake;

GL3Framebuffer::GL3Framebuffer(const GUID& guid, const FramebufferInfo& info) : Framebuffer(guid) {
  Framebuffer::update(info);
}

GL3Framebuffer::~GL3Framebuffer() {
  glDeleteRenderbuffers(1, &_depth_stencil);
  glDeleteFramebuffers(1, &_fbo);
}

unsigned int GL3Framebuffer::id() {
  return _fbo;
}

void GL3Framebuffer::update() {
  if (0 == _fbo) {
    glGenFramebuffers(1, &_fbo);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                                 GL_COLOR_ATTACHMENT3};
  glDrawBuffers(_num_targets, draw_buffers);

  const Vec2i& size = getSize();

  _render_targets.resize(_num_targets);
  for (char i = 0; i < _num_targets; ++i) {
    Ptr<GL3Texture>& render_target = _render_targets[i];

    TexturePropsResource tex_props("", GUID::gen());
    tex_props.setSize(size);
    tex_props.setFormat(_render_target_infos[i].format);

    if (nullptr == render_target) {
      render_target = std::make_shared<GL3Texture>(tex_props);
    } else {
      render_target->update(tex_props);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                           render_target->getId(), 0);
  }

  if (0 == _depth_stencil) {
    glGenRenderbuffers(1, &_depth_stencil);
    glBindRenderbuffer(GL_RENDERBUFFER, _depth_stencil);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              _depth_stencil);
  } else {
    glBindRenderbuffer(GL_RENDERBUFFER, _depth_stencil);
  }
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x(), size.y());
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GL3Framebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);

  if (_depth_test) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const Vec2i& size = getSize();
  glViewport(0, 0, size.x(), size.y());
}

void GL3Framebuffer::preRender() {
  bind();

  if (_auto_clear) {
    for (char i = 0; i < _num_targets; ++i) {
      glClearBufferfv(GL_COLOR, i, _render_target_infos[i].clear_colour.m[0]);
    }
    glClear(GL_DEPTH_BUFFER_BIT);
  }
}

void GL3Framebuffer::blit(const GL3Framebuffer& src) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, src._fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);

  const Vec2i& src_size = src.getSize();
  const Vec2i& size = getSize();
  glBlitFramebuffer(0, 0, src_size.x(), src_size.y(), 0, 0, size.x(), size.y(), GL_COLOR_BUFFER_BIT,
                    GL_LINEAR);
}

OptTextureConstRef GL3Framebuffer::texture(size_t i) const {
  if ((i < _render_targets.size()) && (nullptr != _render_targets[i])) {
    return *_render_targets[i];
  }
  return std::nullopt;
}

void GL3Framebuffer::readPixel(size_t attachment, const Vec2u& position, Vec4u& pixel) const {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
  glReadPixels(position.x(), getSize().y() - position.y(), 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT,
               &pixel.m[0][0]);

  if (const GLenum err = glGetError(); err != GL_NO_ERROR) {
    FEWI::error() << "Failed to read pixel! (" << err << ")";
  }
}