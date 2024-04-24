#include "graphics/framebuffer.hpp"

using namespace pancake;

Framebuffer::Framebuffer(const GUID& guid) : _guid(guid) {}

const Vec2i& Framebuffer::getSize() const {
  return _size;
}

const GUID& Framebuffer::guid() const {
  return _guid;
}

void Framebuffer::setActiveFlag() {
  _active = true;
}

void Framebuffer::clearActiveFlag() {
  _active = false;
}

bool Framebuffer::getActiveFlag() const {
  return _active;
}

void Framebuffer::update(const FramebufferInfo& info) {
  bool update_needed = (_num_targets != info.num_targets) ||
                       (0 != std::memcmp(_render_target_infos, info.render_targets,
                                         info.num_targets * sizeof(RenderTargetInfo)) ||
                        (info.size != _size));

  std::memcpy(_render_target_infos, info.render_targets, sizeof(RenderTargetInfos));
  _num_targets = info.num_targets;
  _blit_priority = info.blit_priority;
  _auto_clear = info.auto_clear;
  _depth_test = info.depth_test;
  _size = info.size;

  if (update_needed) {
    update();
  }
}