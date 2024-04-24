#pragma once

#include "ecs/caching_system.hpp"
#include "ecs/logic_system.hpp"

#include "components/ui.hpp"
#include "ecs/common.hpp"
#include "util/quad_tree.hpp"

#include <memory>

namespace pancake {
using UITrees = std::unordered_map<GUID, std::unique_ptr<QuadTree<Entity>>>;

class BroadcastUI : public CachingSystem<UITrees, LogicSystem> {
 public:
  using CachingSystem<UITrees, LogicSystem>::CachingSystem;

  virtual std::string_view name() const override;
  virtual SystemId id() const override;

  virtual const SessionAccess& getSessionAccess() const override;
  virtual const ComponentAccess& getComponentAccess() const override;
  virtual const MessageAccess& getMessageAccess() const override;

 protected:
  virtual void _run(const SessionWrapper& session,
                    const WorldWrapper& world,
                    UITrees& ui_tree) const override;
};
}  // namespace pancake