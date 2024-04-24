#include "ecs/system_graph.hpp"

#include "core/session_access.hpp"
#include "ecs/encompasser_access.hpp"
#include "ecs/message_access.hpp"
#include "ecs/messages.hpp"
#include "ecs/system.hpp"
#include "ecs/world.hpp"
#include "util/assert.hpp"

#include <algorithm>
#include <map>
#include <set>

using namespace pancake;

class SystemMap
    : std::map<std::pair<SystemId, Ptr<World>>, std::reference_wrapper<SystemGraph::Node>> {
 private:
  using Key = std::pair<SystemId, Ptr<World>>;
  using Map = std::map<Key, std::reference_wrapper<SystemGraph::Node>>;

 public:
  SystemMap() = default;
  ~SystemMap() = default;

  using Map::begin;
  using Map::contains;
  using Map::end;

  void insert(SystemGraph::Node& node) {
    const Ptr<System>& system = node.system();
    const Ptr<World>& world = node.world();

    _session_access = _session_access | system->getSessionAccess();

    ComponentAccess& component_access = _component_accesses[node.world()];
    component_access = component_access | system->getComponentAccess();

    EncompasserAccess& encompasser_access = _encompasser_accesses[node.world()];
    encompasser_access = encompasser_access | system->getEncompasserAccess();

    _global_message_access =
        _global_message_access | (system->getMessageAccess() & MessageAccess::global);
    _message_accesses[world] =
        _message_accesses[world] | (system->getMessageAccess() & MessageAccess::local);

    Key key(node.system()->id(), world);

    const auto dependecy_inserter = [this, &world](SystemId id) {
      Key key{id, world};
      if (!contains(key)) {
        _dependencies.emplace(key);
      }
    };

    std::ranges::for_each(system->precedingSystems(), dependecy_inserter);
    std::ranges::for_each(system->succeedingSystems(), dependecy_inserter);

    _dependencies.erase(key);

    Map::emplace(key, node);
  }

  void insert(const SystemMap& other) {
    for (const auto& [_, system] : other) {
      insert(system);
    }
  }

  bool intersects(const SystemGraph::Node& node) {
    const Ptr<System>& system = node.system();
    const Ptr<World>& world = node.world();
    const ComponentAccess& component_access = _component_accesses[world];
    const EncompasserAccess& encompasser_access = _encompasser_accesses[world];
    const MessageAccess& message_access = _message_accesses[world];

    static const auto has_dependency = [this, &world](SystemId id) {
      return contains({id, world});
    };

    return _session_access.intersects(system->getSessionAccess()) ||
           component_access.intersects(system->getComponentAccess()) ||
           encompasser_access.intersects(system->getEncompasserAccess()) ||
           _global_message_access.intersects(system->getMessageAccess()) ||
           message_access.intersects(system->getMessageAccess()) ||
           std::ranges::any_of(system->precedingSystems(), has_dependency) ||
           std::ranges::any_of(system->succeedingSystems(), has_dependency);
  }

  bool intersects(SystemMap& other) const {
    return _session_access.intersects(other._session_access) ||
           _global_message_access.intersects(other._global_message_access) ||
           std::ranges::any_of(
               _component_accesses,
               [&other](const auto& pair) {
                 return std::get<1>(pair).intersects(other._component_accesses[std::get<0>(pair)]);
               }) ||
           std::ranges::any_of(_encompasser_accesses,
                               [&other](const auto& pair) {
                                 return std::get<1>(pair).intersects(
                                     other._encompasser_accesses[std::get<0>(pair)]);
                               }) ||
           std::ranges::any_of(
               _message_accesses,
               [&other](const auto& pair) {
                 return std::get<1>(pair).intersects(other._message_accesses[std::get<0>(pair)]);
               }) ||
           std::ranges::any_of(_dependencies,
                               [&other](const Key& key) { return other.contains(key); }) ||
           std::ranges::any_of(other._dependencies,
                               [this](const Key& key) { return contains(key); });
  }

  std::vector<std::reference_wrapper<SystemGraph::Node>> sorted() const {
    ensure(_dependencies.empty());

    std::vector<std::reference_wrapper<SystemGraph::Node>> sorted_nodes;
    std::set<Key> processed_nodes;
    while (processed_nodes.size() != Map::size()) {
      bool insert_made = false;
      for (const auto& [key, node_ref] : *this) {
        if (processed_nodes.contains(key)) {
          continue;
        }

        const SystemGraph::Node& node = node_ref.get();
        const Ptr<System>& system = node.system();
        System::SystemIdSet preceding_systems = system->precedingSystems();
        System::SystemIdSet succeeding_systems = system->succeedingSystems();

        auto min_it = sorted_nodes.begin();
        auto max_it = sorted_nodes.end();
        bool preceding_and_succeeding_present =
            !(preceding_systems.empty() || succeeding_systems.empty());

        for (auto it = sorted_nodes.begin(); it != sorted_nodes.end(); ++it) {
          const SystemGraph::Node& sorted_node = *it;

          if (node.world() != sorted_node.world()) {
            continue;
          }

          const Ptr<System>& sorted_system = sorted_node.system();

          if (const auto set_it = preceding_systems.find(sorted_system->id());
              (preceding_systems.end() != set_it)) {
            preceding_systems.erase(set_it);
            min_it = it;
          }

          if (const auto set_it = succeeding_systems.find(sorted_system->id());
              (succeeding_systems.end() != set_it)) {
            succeeding_systems.erase(set_it);
            if (max_it == sorted_nodes.end()) {
              max_it = it;
            }
          }
        }

        if (preceding_systems.empty() && succeeding_systems.empty()) {
          ensure((!preceding_and_succeeding_present) || (min_it < max_it));

          sorted_nodes.insert(max_it, node_ref);
          processed_nodes.insert(key);
          insert_made = true;
        }
      }

      if (!insert_made) {
        // will also be thrown if a system lists another as
        // preceding and that lists the former as succeeding as well!
        FEWI::fatal("systems have conflicting dependencies!");
        break;
      }
    }
    return sorted_nodes;
  }

 private:
  SessionAccess _session_access;
  MessageAccess _global_message_access;
  std::map<Ptr<World>, ComponentAccess> _component_accesses;
  std::map<Ptr<World>, EncompasserAccess> _encompasser_accesses;
  std::map<Ptr<World>, MessageAccess> _message_accesses;
  std::set<Key> _dependencies;
};

SystemGraph::Node::Node(SystemNodeId id, const Ptr<System>& system, const Ptr<World>& world)
    : _id(id), _system(system), _world(world) {}

SystemNodeId SystemGraph::Node::id() const {
  return _id;
}

const Ptr<System>& SystemGraph::Node::system() const {
  return _system;
}

const Ptr<World>& SystemGraph::Node::world() const {
  return _world;
}

const SystemGraph::NodeSet& SystemGraph::Node::dependents() const {
  return _dependents;
}

const SystemGraph::NodeSet& SystemGraph::Node::dependencies() const {
  return _dependencies;
}

bool SystemGraph::Node::intersects(const Node& other) const {
  if ((_world == other._world) && _system->intersects(*other._system)) {
    return true;
  }

  const MessageAccess& msg_access = _system->getMessageAccess();
  const MessageAccess& other_sig_access = other._system->getMessageAccess();
  for (const MessageId send_sig_id : msg_access.getSends()) {
    if (Messages::isGlobal(send_sig_id) &&
        (other_sig_access.getReceives()[send_sig_id] || other_sig_access.getSends()[send_sig_id])) {
      return true;
    }
  }
  for (const MessageId other_send_sig_id : other_sig_access.getSends()) {
    if (Messages::isGlobal(other_send_sig_id) &&
        (msg_access.getReceives()[other_send_sig_id] || msg_access.getSends()[other_send_sig_id])) {
      return true;
    }
  }

  return false;
}

void SystemGraph::build(const Systems& systems, const std::set<Ptr<World>>& worlds) {
  _nodes.clear();
  _roots.clear();

  for (const Ptr<System>& system : systems) {
    if (ComponentAccess::empty == system->getComponentAccess()) {
      _nodes.emplace_back(static_cast<SystemNodeId>(_nodes.size()), system, nullptr);
    } else {
      for (const Ptr<World>& world : worlds) {
        _nodes.emplace_back(static_cast<SystemNodeId>(_nodes.size()), system, world);
      }
    }
  }

  // roughly bin nodes into buckets by access and dependencies
  std::list<SystemMap> system_maps;
  for (Node& node : _nodes) {
    bool inserted = false;
    for (SystemMap& system_map : system_maps) {
      if (system_map.intersects(node)) {
        system_map.insert(node);
        inserted = true;
        break;
      }
    }
    if (!inserted) {
      system_maps.emplace_back().insert(node);
    }
  }

  // merge intersecting buckets until disjoint
  bool merged = true;
  while (merged) {
    merged = false;
    for (std::list<SystemMap>::iterator it_a = system_maps.begin(); it_a != system_maps.end();
         ++it_a) {
      for (std::list<SystemMap>::iterator it_b = it_a;
           (it_b != system_maps.end()) && (++it_b != system_maps.end());) {
        if (it_a->intersects(*it_b)) {
          merged = true;
          it_a->insert(*it_b);
          it_b = system_maps.erase(it_b);
        } else {
          ++it_b;
        }
      }
    }
  }

  // insert disjoint maps as disjoint linear graphs
  for (const SystemMap& system_map : system_maps) {
    Node* root = nullptr;
    Node* prev_node = nullptr;
    for (Node& node : system_map.sorted()) {
      if (prev_node == nullptr) {
        _roots.insert(node);
        root = &node;
      } else {
        prev_node->_dependents.insert(node);
        node._dependencies.insert(*prev_node);
      }

      prev_node = &node;
    }
  }

  static bool (*branchIfPossible)(Node&, NodeSet&) = [](Node& node, NodeSet& roots) -> bool {
    bool branched = false;
    for (Node& dependency : node._dependencies) {
      if (!dependency.intersects(node)) {
        std::ranges::copy(dependency._dependencies,
                          std::inserter(node._dependencies, node._dependencies.begin()));
        for (Node& dependency2 : dependency._dependencies) {
          dependency2._dependents.emplace(node);
        }

        dependency._dependents.erase(node);
        node._dependencies.erase(dependency);

        if (node._dependencies.empty()) {
          roots.emplace(node);
        }

        std::ranges::copy(node._dependents,
                          std::inserter(dependency._dependents, dependency._dependents.begin()));
        for (Node& dependent : node._dependents) {
          dependent._dependencies.emplace(dependency);
        }

        branched = true;
        break;
      }
    }

    for (Node& dependent : node._dependents) {
      if (branchIfPossible(dependent, roots)) {
        return true;
      }
    }

    return branched;
  };

  bool branched = true;
  while (branched) {
    branched = false;
    for (Node& root : _roots) {
      branched = branchIfPossible(root, _roots) || branched;
    }
  }
}

const SystemGraph::Nodes& SystemGraph::nodes() {
  return _nodes;
}

const SystemGraph::NodeSet& SystemGraph::roots() {
  return _roots;
}

size_t SystemGraph::size() const {
  return _nodes.size();
}

void SystemGraph::visualise(std::ostream& out) const {
  static void (*visualiser)(std::ostream&, Node&, NodeSet&) = [](std::ostream& out, Node& node,
                                                                 NodeSet& visualised) {
    if (visualised.contains(node)) {
      return;
    }
    visualised.insert(node);

    out << " " << node._system->name() << std::endl;
    for (Node& dependent : node._dependents) {
      out << " " << node._system->name() << " -> " << dependent._system->name() << std::endl;
      visualiser(out, dependent, visualised);
    }
  };

  out << "digraph G {" << std::endl;

  NodeSet visualised;
  for (Node& root : _roots) {
    visualiser(out, root, visualised);
  }

  out << "}" << std::endl;
}