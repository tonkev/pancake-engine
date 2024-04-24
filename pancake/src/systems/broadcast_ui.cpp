#include "systems/broadcast_ui.hpp"

#include "components/core.hpp"
#include "components/ui.hpp"
#include "core/input.hpp"
#include "core/renderer.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/world_wrapper.hpp"
#include "messages/ui.hpp"

using namespace pancake;

LogicSystem::StaticAdder<BroadcastUI> broadcast_ui_adder{};

void BroadcastUI::_run(const SessionWrapper& session,
                       const WorldWrapper& world,
                       UITrees& ui_trees) const {
  for (auto& [framebuffer, ui_tree] : ui_trees) {
    ui_tree->clear();
  }

  for (const auto& [base, ui] : world.getComponents<const Base, UIContainer>()) {
    GUID framebuffer = GUID::null;
    Vec2f fb_centre = Vec2f(250.f);
    float fb_size = 500.f;
    if (auto fb_opt = world.getEntityWrapper(base->self)
                          .getArchetypeParent<const Base, const FramebufferInfo>();
        fb_opt.has_value()) {
      const auto& [fb_base, fb_info] = fb_opt.value();
      framebuffer = fb_base->guid;
      fb_centre = Vec2f(fb_info->size) * 0.5f;
      fb_size = static_cast<float>(std::max(fb_info->size.x(), fb_info->size.y()));
    }

    QuadTree<Entity>* ui_tree = nullptr;
    if (auto it = ui_trees.find(framebuffer); it != ui_trees.end()) {
      ui_tree = it->second.get();
    } else {
      ui_tree = ui_trees.emplace(framebuffer, new QuadTree<Entity>(fb_centre, fb_size, 10.f))
                    .first->second.get();
    }

    const Vec2f extents = ui->absolute_size * 0.5f;
    ui_tree->insert(ui->absolute_position + extents, extents, base->self);
  }

  const Input& input = session.input();
  bool mouse_clicked = input.isMouseJustPressed(MouseCode::Left);

  QuadTree<Entity>::Hit hit;
  const Vec2f mouse_pos = session.renderer().screenToRenderPosition(input.getMousePosition());

  for (const auto& [framebuffer, ui_tree] : ui_trees) {
    ui_tree->pointSweep(mouse_pos, [&session, &mouse_clicked](const Entity& ent) {
      session.sendMessage<UIOnHover>({ent});
      if (mouse_clicked) {
        session.sendMessage<UIOnClick>({ent});
      }
    });
  }

  /*
  for (auto& [framebuffer, ui_tree] : ui_trees) {
    delete ui_tree if empty
  }
  */
}

std::string_view BroadcastUI::name() const {
  return "BroadcastUI";
}

SystemId BroadcastUI::id() const {
  return System::id<BroadcastUI>();
}

const SessionAccess& BroadcastUI::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addRenderer();
  return session_access;
}

const ComponentAccess& BroadcastUI::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Base, UIContainer, const FramebufferInfo>();
  return component_access;
}

const MessageAccess& BroadcastUI::getMessageAccess() const {
  static const MessageAccess message_access = Messages::getAccess<UIOnHover, UIOnClick>();
  return message_access;
}