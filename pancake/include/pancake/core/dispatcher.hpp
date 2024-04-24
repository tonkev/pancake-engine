#pragma once

#include "ecs/system_graph.hpp"
#include "util/containers.hpp"

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace pancake {
class Dispatcher {
 public:
  Dispatcher();
  ~Dispatcher();

  void execute(SystemGraph& system_graph);

 private:
  void consumeSystems();

  bool _online;

  std::queue<std::reference_wrapper<SystemGraph::Node>> _work_queue;
  std::mutex _work_mutex;

  SystemGraph::NodeSet _done_set;
  std::mutex _done_mutex;

  std::atomic_size_t _num_done;

  std::vector<std::thread> _thread_pool;
};
}  // namespace pancake