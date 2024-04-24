#include "systems/simple_collision_handler.hpp"

#include "components/2d.hpp"
#include "core/session_wrapper.hpp"
#include "ecs/component_access.hpp"
#include "ecs/encompasser_access.hpp"
#include "ecs/world_wrapper.hpp"
#include "encompassers/physics_state.hpp"
#include "util/aabb.hpp"

using namespace pancake;

const LogicSystem::StaticAdder<SimpleCollisionHandler> simple_collision_handler_adder{};

void SimpleCollisionHandler::_run(const SessionWrapper& session, const WorldWrapper& world) const {
  PhysicsState* physics_state = world.getEncompasser<PhysicsState>(GUID::null);
  if (nullptr == physics_state) {
    world.addEncompasser<PhysicsState>(std::make_unique<PhysicsState>(GUID::null));
    physics_state = world.getEncompasser<PhysicsState>(GUID::null);
  }
  if (nullptr == physics_state) {
    return;
  }
  physics_state->clear();

  for (const auto& [base, transform, collider, body] :
       world.getComponents<const Base, const Transform2D, const RectangleCollider2D,
                           const StaticBody2D>()) {
    physics_state->addBody(transform->translation(), transform->scale().mask(collider->extents),
                           base->self);
  }

  Vec2f overlap, exit;
  for (const auto& [base_a, transform_a, collider_a, body_a] :
       world.getComponents<const Base, Transform2D, const RectangleCollider2D,
                           const KinematicBody2D>()) {
    const Vec2f extents_a = transform_a->scale().mask(collider_a->extents);
    const Vec2f dir = body_a->velocity.sign();

    Vec2i slide_mask = Vec2i::ones().mask(dir.abs());
    bool biaxis = (slide_mask.x() == slide_mask.y()) && (1 == slide_mask.x());
    Vec2f remaining = body_a->velocity * session.delta();
    int loop_count = 0;

    while (0.00001f < remaining.squaredNorm()) {
      Vec2f min_step = remaining.mask(slide_mask);
      Vec2f next_slide_mask = slide_mask;
      CollisionInfo min_coll_info;
      bool collided = false;

      QuadTree<Entity>::Hit hit;
      if (physics_state->aabbCast(transform_a->translation(), extents_a, min_step, min_step.norm(),
                                  hit)) {
        min_step = hit.hit - transform_a->translation();

        min_coll_info.normal = hit.normal;
        min_coll_info.other = hit.element;

        if (0.001f < std::abs(hit.normal.x())) {
          next_slide_mask = Vec2i(0, 1);
        } else {
          next_slide_mask = Vec2i(1, 0);
        }

        collided = true;
      }

      transform_a->modify().localTranslation() += min_step;

      if (collided) {
        physics_state->addCollision(base_a->self, min_coll_info);
      }

      if ((1 < loop_count) && (min_step.squaredNorm() < 0.00001f)) {
        break;
      }
      ++loop_count;

      remaining -= min_step;
      slide_mask = slide_mask.mask(next_slide_mask);
      if (Vec2i::zeros() == slide_mask) {
        if (biaxis) {
          slide_mask = Vec2i::ones();
        } else {
          break;
        }
      }
    }
  }
}

std::string_view SimpleCollisionHandler::name() const {
  return "SimpleCollisionHandler";
}

SystemId SimpleCollisionHandler::id() const {
  return System::id<SimpleCollisionHandler>();
}

const ComponentAccess& SimpleCollisionHandler::getComponentAccess() const {
  static const ComponentAccess component_access =
      Components::getAccess<const Base, Transform2D, const RectangleCollider2D,
                            const KinematicBody2D, const StaticBody2D>();
  return component_access;
}

const EncompasserAccess& SimpleCollisionHandler::getEncompasserAccess() const {
  static const EncompasserAccess encompasser_access = Encompassers::getAccess<PhysicsState>();
  return encompasser_access;
}
