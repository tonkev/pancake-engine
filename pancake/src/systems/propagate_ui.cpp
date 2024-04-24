#include "systems/propagate_ui.hpp"

#include "components/core.hpp"
#include "components/ui.hpp"
#include "core/renderer.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/world_wrapper.hpp"

using namespace pancake;

LogicSystem::StaticAdder<PropagateUI> propagate_ui_adder{};

void PropagateUI::recursor(int axis,
                           UIContainer& ui,
                           const Entity& ui_ent,
                           const UIContainer& parent_ui,
                           const WorldWrapper& world) {
  float position = ui.position.m[0][axis];
  const float size = ui.size.m[0][axis];
  const float parent_absolute_position = parent_ui.absolute_position.m[0][axis];
  const float parent_absolute_size = parent_ui.absolute_size.m[0][axis];
  const UIContainer::AnchoringType anchoring_type = ui.getAnchoringType(axis);
  const UIContainer::PositioningType positioning_type = ui.getPositioningType(axis);
  const UIContainer::SizingType sizing_type = ui.getSizingType(axis);

  float& absolute_size = ui.absolute_size.m[0][axis];
  switch (sizing_type) {
    case UIContainer::SizingType::Fixed:
      absolute_size = size;
      break;
    case UIContainer::SizingType::Relative:
      absolute_size = parent_absolute_size * size;
      break;
  }

  switch (anchoring_type) {
    case UIContainer::AnchoringType::Start:
      break;
    case UIContainer::AnchoringType::Centre:
      position -= absolute_size * 0.5f;
      break;
    case UIContainer::AnchoringType::End:
      position -= absolute_size;
      break;
  }

  float& absolute_position = ui.absolute_position.m[0][axis];
  switch (positioning_type) {
    case UIContainer::PositioningType::FixedFromStart:
      absolute_position = parent_absolute_position + position;
      break;
    case UIContainer::PositioningType::FixedFromCentre:
      absolute_position = parent_absolute_position + (parent_absolute_size * 0.5f) + position;
      break;
    case UIContainer::PositioningType::FixedFromEnd:
      absolute_position = parent_absolute_position + parent_absolute_size + position;
      break;
    case UIContainer::PositioningType::Relative:
      absolute_position = parent_absolute_size * position;
  }

  for (const auto& [child_base, child_ui] :
       world.getChildrenComponents<const Base, UIContainer>(ui_ent)) {
    switch (child_ui->getSizingType(axis)) {
      case UIContainer::SizingType::Fixed:
      case UIContainer::SizingType::Relative:
        recursor(axis, *child_ui, child_base->self, ui, world);
        break;
    }
  }
}

void PropagateUI::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  UIContainer default_ui;
  default_ui.absolute_position = Vec2f::zeros();
  default_ui.absolute_size = session.renderer().renderSize();

  for (const auto& [root_base, root_ui] : world.getComponents<const Base, UIContainer>()) {
    if ((Entity::null == root_base->parent) ||
        (!world.hasComponent<UIContainer>(root_base->parent))) {
      UIContainer frame_ui = default_ui;
      if (const auto fb_info_opt =
              world.getEntityWrapper(root_base->self).getArchetypeParent<FramebufferInfo>();
          fb_info_opt.has_value()) {
        const auto& [fb_info] = fb_info_opt.value();
        frame_ui.absolute_position = Vec2f::zeros();
        frame_ui.absolute_size = fb_info->size;
      }

      recursor(0, *root_ui, root_base->self, frame_ui, world);
      recursor(1, *root_ui, root_base->self, frame_ui, world);
    }
  }
}

std::string_view PropagateUI::name() const {
  return "PropagateUI";
}

SystemId PropagateUI::id() const {
  return System::id<PropagateUI>();
}

const SessionAccess& PropagateUI::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& PropagateUI::getComponentAccess() const {
  static const ComponentAccess component_access = Components::getAccess<const Base, UIContainer>();
  return component_access;
}
