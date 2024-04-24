#pragma once

#include "ecs/system.hpp"
#include "pancake.hpp"
#include "util/containers.hpp"

#include <list>
#include <set>
#include <unordered_set>
#include <vector>

namespace pancake {
class SystemGraph {
 public:
  class Node;
  using Nodes = std::list<Node>;
  using NodeSet = std::unordered_set<std::reference_wrapper<Node>,
                                     HashRefWrapperByAddr<Node>,
                                     EqualToRefWrapperByAddr<Node>>;

  class Node {
   public:
    Node(SystemNodeId id, const Ptr<System>& system, const Ptr<World>& world);

    SystemNodeId id() const;
    const Ptr<System>& system() const;
    const Ptr<World>& world() const;
    const NodeSet& dependents() const;
    const NodeSet& dependencies() const;

    bool intersects(const Node& other) const;

   private:
    friend SystemGraph;

    SystemNodeId _id;
    Ptr<System> _system;
    Ptr<World> _world;
    NodeSet _dependents;
    NodeSet _dependencies;
  };

  SystemGraph() = default;
  ~SystemGraph() = default;

  void build(const Systems& systems, const std::set<Ptr<World>>& worlds);

  const Nodes& nodes();
  const NodeSet& roots();

  size_t size() const;

  void visualise(std::ostream& out) const;

 private:
  Nodes _nodes;
  NodeSet _roots;
};
}  // namespace pancake