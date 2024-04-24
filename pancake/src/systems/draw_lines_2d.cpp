#include "systems/draw_lines_2d.hpp"

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

const DrawSystem::StaticAdder<DrawLines2D> draw_lines_2d_adder{};

void DrawLines2D::_configure(Session& session) {
  Renderer& renderer = session.renderer();
  _mesh = renderer.getUnitSquare()->guid();
  _shader = renderer.getDefaultShader()->guid();
}

void DrawLines2D::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  static const TextureRef blank_tex;

  for (const auto& [line, points] : world.getComponents<const LineRenderer2D, const Points2D>()) {
    const Vec2f* prev_point = nullptr;
    for (const Vec2f& point : *points) {
      if (nullptr != prev_point) {
        Transform2D transform;
        {
          const Vec2f& a = *prev_point;
          const Vec2f& b = point;
          Transform2D::Accessor accessor = transform.modify();
          accessor.translation() = (a + b) * 0.5f;
          accessor.rotation() = (b - a).atan2();
          accessor.scale().x() = (b - a).norm();
          accessor.scale().y() = line->width;
        }

        session.renderer().submit(
            line->camera_mask, GUID::null,
            {ShaderInput("colour", line->colour), ShaderInput("tex", blank_tex)}, _mesh,
            transform.matrix3D());
      }
      prev_point = &point;
    }
  }
}

std::string_view DrawLines2D::name() const {
  return "DrawLines2D";
}

SystemId DrawLines2D::id() const {
  return System::id<DrawLines2D>();
}

const SessionAccess& DrawLines2D::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& DrawLines2D::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const LineRenderer2D, const Points2D>();
  return component_access;
}