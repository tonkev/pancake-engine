#include "ecs/default_components.hpp"

#include "components/2d.hpp"
#include "components/3d.hpp"
#include "components/core.hpp"
#include "components/ui.hpp"
#include "util/type_desc_library.hpp"

using namespace pancake;

std::span<std::reference_wrapper<const TypeDesc>> pancake::default_components::get() {
  static std::reference_wrapper<const TypeDesc> descs[] = {
      TypeDescLibrary::get<Base>(),
      TypeDescLibrary::get<FramebufferInfo>(),
      TypeDescLibrary::get<Transform2D>(),
      TypeDescLibrary::get<Sprite2D>(),
      TypeDescLibrary::get<Camera2D>(),
      TypeDescLibrary::get<StaticBody2D>(),
      TypeDescLibrary::get<KinematicBody2D>(),
      TypeDescLibrary::get<RectangleCollider2D>(),
      TypeDescLibrary::get<Points2D>(),
      TypeDescLibrary::get<LineRenderer2D>(),
      TypeDescLibrary::get<Transform3D>(),
      TypeDescLibrary::get<MeshInstance>(),
      TypeDescLibrary::get<MaterialInstance>(),
      TypeDescLibrary::get<PointLight>(),
      TypeDescLibrary::get<Camera3D>(),
      TypeDescLibrary::get<UIContainer>(),
      TypeDescLibrary::get<UIBackground>(),
      TypeDescLibrary::get<UIString>(),
      TypeDescLibrary::get<UIText>(),
      TypeDescLibrary::get<Text>()};
  return descs;
}