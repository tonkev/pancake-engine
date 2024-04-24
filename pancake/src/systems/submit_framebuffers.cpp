#include "systems/submit_framebuffers.hpp"

#include "components/core.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<SubmitFramebuffers> submit_framebuffers_adder{};

void SubmitFramebuffers::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  const Vec2f render_size = renderer.renderSize();
  for (const auto& [base, fb_info] : world.getComponents<const Base, FramebufferInfo>()) {
    if ((0 < fb_info->relative_size.x()) || (0 < fb_info->relative_size.y())) {
      fb_info->size = render_size.mask(fb_info->relative_size);
    }

    renderer.submitFramebuffer(base->guid, *fb_info);
  }
}

std::string_view SubmitFramebuffers::name() const {
  return "SubmitFramebuffers";
}

SystemId SubmitFramebuffers::id() const {
  return System::id<SubmitFramebuffers>();
}

const SessionAccess& SubmitFramebuffers::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& SubmitFramebuffers::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Base, FramebufferInfo>();
  return component_access;
}