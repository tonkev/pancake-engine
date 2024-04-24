#pragma once

#include "components/core.hpp"
#include "graphics/common.hpp"
#include "graphics/texture.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"

namespace pancake {
class Texture;
class Framebuffer {
 public:
  virtual ~Framebuffer() = default;

  const Vec2i& getSize() const;
  const GUID& guid() const;

  void setActiveFlag();
  void clearActiveFlag();
  bool getActiveFlag() const;

  void update(const FramebufferInfo& info);

  virtual void bind() = 0;
  virtual void preRender() = 0;

  virtual OptTextureConstRef texture(size_t i = 0) const = 0;
  virtual void readPixel(size_t attachment, const Vec2u& position, Vec4u& pixel) const = 0;

 protected:
  Framebuffer(const GUID& guid);

  virtual void update() = 0;

  RenderTargetInfos _render_target_infos = {};
  int _blit_priority = -1;
  char _num_targets = 1;
  bool _auto_clear = true;
  bool _depth_test = true;

 private:
  GUID _guid = GUID::null;
  Vec2i _size = Vec2i::zeros();
  bool _active = true;
};
}  // namespace pancake