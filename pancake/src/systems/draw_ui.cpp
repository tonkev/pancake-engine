#include "systems/draw_ui.hpp"

#include "components/ui.hpp"
#include "core/renderer.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "core/window.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "graphics/shader_input.hpp"
#include "graphics/texture.hpp"
#include "resources/tileset_resource.hpp"
#include "util/fewi.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<DrawUI> draw_ui_adder{};

void recursor(const Entity& ent,
              const UIContainer& ui,
              const GUID& framebuffer,
              const WorldWrapper& world,
              const Mat4f& projection_view,
              const GUID& mesh,
              const GUID& shader,
              Renderer& renderer,
              Resources& resources) {
  EntityWrapper entity = world.getEntityWrapper(ent);

  CommonPerInstanceData cpid;
  if (entity.hasComponent<UIBackground>()) {
    const UIBackground& bg = entity.getComponent<const UIBackground>();
    cpid.mvp_transform =
        projection_view *
        Mat4f::translation(ui.absolute_position.x() + (ui.absolute_size.x() * 0.5f),
                           ui.absolute_position.y() + (ui.absolute_size.y() * 0.5f), 0.f) *
        Mat4f::scale(ui.absolute_size.x(), -ui.absolute_size.y(), 1.f);
    renderer.submit(1000, framebuffer, {}, shader,
                    {ShaderInput("colour", bg.colour), ShaderInput("tex", TextureRef())}, mesh,
                    cpid);
  }

  if (entity.hasComponents<UIText, UIString>()) {
    const UIText& text = entity.getComponent<const UIText>();
    const UIString& str = entity.getComponent<const UIString>();

    Vec2f font_extents = text.font_size * 0.5f;
    Vec2f position = Vec2f(-font_extents.x(), font_extents.y());

    for (const char c : str) {
      if (c == '\0') {
        break;
      }

      position.x() += text.font_size.x();
      if (c == '\n') {
        position.x() = font_extents.x();
        position.y() += text.font_size.y();
      }

      if (ui.absolute_size.x() < (position.x() + font_extents.x())) {
        if (text.wraparound) {
          position.x() = font_extents.x();
          position.y() += text.font_size.y();
        } else if (text.cutoff) {
          continue;
        }
      }

      if ((ui.absolute_size.y() < (position.y() + font_extents.y())) && text.cutoff) {
        break;
      }

      Vec2f absolute_position = ui.absolute_position + position;
      cpid.mvp_transform = projection_view * Mat4f::translation(Vec3f(absolute_position, 0.f)) *
                           Mat4f::scale(text.font_size.x(), -text.font_size.y(), 1.f);
      renderer.submit(1000, framebuffer, {}, shader,
                      {ShaderInput("colour", Vec4f(1.f, 0.f, 0.f, 1.f)),
                       ShaderInput("tex", TextureRef(text.font, c))},
                      mesh, cpid);
    }
  }

  for (const auto& [child_base, child_ui] :
       world.getChildrenComponents<const Base, const UIContainer>(ent)) {
    recursor(child_base->self, *child_ui, framebuffer, world, projection_view, mesh, shader,
             renderer, resources);
  }
}

void DrawUI::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  Resources& resources = session.resources();
  GUID shader = renderer.getDefaultShader()->guid();
  GUID mesh = renderer.getUnitSquare()->guid();

  static const auto gen_projection_view = [](const Vec2f& size) {
    return Mat4f::ortographic(size.x(), -size.y(), -1.f) *
           Mat4f::translation(size.x() * -0.5f, size.y() * -0.5f, 0.f);
  };

  const Mat4f default_projection_view = gen_projection_view(renderer.renderSize());
  for (const auto& [base, ui] : world.getComponents<const Base, const UIContainer>()) {
    if (const auto ui_parent_opt =
            world.getEntityWrapper(base->parent).getArchetypeParent<UIContainer>();
        ui_parent_opt.has_value()) {
      continue;
    }

    GUID framebuffer = GUID::null;
    Mat4f projection_view = default_projection_view;
    if (const auto fb_info_opt =
            world.getEntityWrapper(base->self).getArchetypeParent<Base, FramebufferInfo>();
        fb_info_opt.has_value()) {
      const auto& [fb_base, fb_info] = fb_info_opt.value();
      framebuffer = fb_base->guid;
      projection_view = gen_projection_view(fb_info->size);
    }

    recursor(base->self, *ui, framebuffer, world, projection_view, mesh, shader, renderer,
             resources);
  }
}

std::string_view DrawUI::name() const {
  return "DrawUI";
}

SystemId DrawUI::id() const {
  return System::id<DrawUI>();
}

const SessionAccess& DrawUI::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer().addResources();
  return session_access;
}

const ComponentAccess& DrawUI::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Base, const UIContainer, const UIBackground, const UIText,
                            const UIString>();
  return component_access;
}