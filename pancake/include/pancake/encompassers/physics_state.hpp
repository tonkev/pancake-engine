#include "ecs/encompasser.hpp"

#include "components/common.hpp"
#include "ecs/common.hpp"
#include "util/matrix.hpp"
#include "util/quad_tree.hpp"

#include <unordered_map>

namespace pancake {
struct CollisionInfo {
  Entity other;
  Vec2f normal;
};

class PhysicsState : public Encompasser {
 public:
  PhysicsState(const GUID& guid);
  virtual ~PhysicsState() = default;

  void addBody(const Vec2f& centre, const Vec2f& extents, const Entity& entity);
  void addCollision(const Entity& entity, const CollisionInfo& info);
  void clear();

  bool aabbCast(const Vec2f& start,
                const Vec2f& extents,
                const Vec2f& dir,
                float max_length,
                QuadTree<Entity>::Hit& hit) const;

  bool rayCast(const Vec2f& start,
               const Vec2f& dir,
               float max_length,
               QuadTree<Entity>::Hit& hit) const;

  const std::vector<CollisionInfo>& getCollisions(const Entity& entity) const;

 private:
  QuadTree<Entity> _bodies;
  std::unordered_map<Entity, std::vector<CollisionInfo>> _collisions;
};
}  // namespace pancake