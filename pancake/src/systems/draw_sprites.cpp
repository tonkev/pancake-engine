#include "systems/draw_sprites.hpp"

#include "components/2d.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "graphics/shader_input.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<DrawSprites> draw_sprites_adder{};

void DrawSprites::_configure(Session& session) {
  Renderer& renderer = session.renderer();
  _mesh = renderer.getUnitSquare()->guid();
  _shader = renderer.getDefaultShader()->guid();
}

void DrawSprites::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();

  for (const auto& [transform, sprite] : world.getComponents<const Transform2D, const Sprite2D>()) {
    Mat4f model = transform->matrix3D();
    session.renderer().submit(
        sprite->camera_mask, GUID::null,
        {ShaderInput("colour", Vec4f(1.f, 1.f, 1.f, 1.f)), ShaderInput("tex", sprite->texture)},
        _mesh, model);
  }
}

std::string_view DrawSprites::name() const {
  return "DrawSprites";
}

SystemId DrawSprites::id() const {
  return System::id<DrawSprites>();
}

const SessionAccess& DrawSprites::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& DrawSprites::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Transform2D, const Sprite2D>();
  return component_access;
}