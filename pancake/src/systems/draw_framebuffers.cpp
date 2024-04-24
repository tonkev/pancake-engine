#include "systems/draw_framebuffers.hpp"

#include "components/3d.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"
#include "graphics/shader.hpp"
#include "graphics/shader_input.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<DrawFramebuffers> draw_sprites_adder{};

void DrawFramebuffers::_run(const SessionWrapper& session, const WorldWrapper& world) const {}

std::string_view DrawFramebuffers::name() const {
  return "DrawFramebuffers";
}

SystemId DrawFramebuffers::id() const {
  return System::id<DrawFramebuffers>();
}

const SessionAccess& DrawFramebuffers::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& DrawFramebuffers::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const FramebufferInfo, const MeshInstance>();
  return component_access;
}