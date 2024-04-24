#include "systems/draw_texts.hpp"

#include "components/2d.hpp"
#include "components/ui.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "core/window.hpp"
#include "ecs/components.hpp"
#include "ecs/world_wrapper.hpp"
#include "gl3/gl3_renderer.hpp"
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "graphics/shader_input.hpp"
#include "graphics/texture.hpp"
#include "resources/resources.hpp"
#include "resources/text_resource.hpp"
#include "resources/tileset_resource.hpp"
#include "util/fewi.hpp"

using namespace pancake;

const DrawSystem::StaticAdder<DrawTexts> draw_texts_adder{};

void DrawTexts::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  Renderer& renderer = session.renderer();
  Resources& resources = session.resources();
  GUID shader = renderer.getDefaultShader()->guid();
  GUID mesh = renderer.getUnitSquare()->guid();

  for (const auto& [transform, text] : world.getComponents<const Transform2D, const Text>()) {
    const auto text_opt = resources.getOrCreate<TextResource>(text->text);
    const auto font_opt = resources.getOrCreate<TilesetResource>(text->font);
    if (text_opt.has_value() && font_opt.has_value()) {
      int i = 0;
      Mat4f origin_model = Mat4f::scale(Vec3f(text->font_size, 1.0f)) * transform->matrix3D();
      for (const char c : text_opt.value().get().constText()) {
        renderer.submit(
            text->camera_mask, GUID::null,
            {ShaderInput("colour", Vec4f(1.f, 1.f, 1.f, 1.f)),
             ShaderInput("tex", TextureRef(text->font, c))},
            mesh, origin_model * Mat4f::translation(Vec3f(static_cast<float>(i++), 0.f, 0.f)));
      }
    }
  }
}

std::string_view DrawTexts::name() const {
  return "DrawTexts";
}

SystemId DrawTexts::id() const {
  return System::id<DrawTexts>();
}

const SessionAccess& DrawTexts::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer().addResources();
  return session_access;
}

const ComponentAccess& DrawTexts::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Transform2D, const Text, const Camera2D>();
  return component_access;
}