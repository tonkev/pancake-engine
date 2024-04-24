#include "core/dispatcher.hpp"

#include "ecs/system_graph.hpp"
#include "util/assert.hpp"

using namespace pancake;

const std::chrono::milliseconds THREAD_SLEEP_TIME(1);

Dispatcher::Dispatcher() : _online(true), _num_done(0) {
  const int num_threads = std::thread::hardware_concurrency() - 1;
  _thread_pool.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i) {
    _thread_pool.emplace_back([this]() {
      while (_online) {
        consumeSystems();
        std::this_thread::sleep_for(THREAD_SLEEP_TIME);
      }
    });
  }
}

Dispatcher::~Dispatcher() {
  _online = false;
  for (std::thread& thread : _thread_pool) {
    thread.join();
  }
}

void Dispatcher::execute(SystemGraph& system_graph) {
  ensure(_work_queue.empty());

  _num_done = 0;
  _done_set.clear();

  {
    std::scoped_lock work_lock(_work_mutex);
    for (SystemGraph::Node& root : system_graph.roots()) {
      _work_queue.push(root);
    }
  }

  const size_t system_graph_size = system_graph.size();
  while (_num_done < system_graph_size) {
    consumeSystems();
  }

  ensure(_work_queue.empty());
  ensure(_num_done == system_graph_size);
}

void Dispatcher::consumeSystems() {
  SystemGraph::Node* node = nullptr;

  {
    std::scoped_lock work_lock(_work_mutex);
    if (!_work_queue.empty()) {
      node = &_work_queue.front().get();
      _work_queue.pop();
    }
  }

  while (nullptr != node) {
    const Ptr<World>& world = node->world();
    if (world) {
      node->system()->run(*world, node->id());
    } else {
      node->system()->run(node->id());
    }

    SystemGraph::Node* next_node = nullptr;
    {
      std::scoped_lock done_lock(_done_mutex);
      std::unique_lock work_lock(_work_mutex, std::defer_lock);

      _done_set.insert(*node);
      for (SystemGraph::Node& dependent : node->dependents()) {
        bool dependencies_resolved = true;
        for (SystemGraph::Node& dependency : dependent.dependencies()) {
          if (!_done_set.contains(dependency)) {
            dependencies_resolved = false;
            break;
          }
        }
        if (!dependencies_resolved) {
          continue;
        }

        if (nullptr == next_node) {
          next_node = &dependent;
        } else {
          if (!work_lock.owns_lock()) {
            work_lock.lock();
          }

          _work_queue.push(dependent);
        }
      }
    }

    ++_num_done;

    node = next_node;
  }
}