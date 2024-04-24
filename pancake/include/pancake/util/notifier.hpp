#pragma once

#include "pancake.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace pancake {
class Notifier {
 private:
  struct ReceptorCallback {
    std::weak_ptr<void> receptor;
    std::function<void()> callback;
  };

 public:
  void subscribe(WeakPtr<void> receptor, const std::function<void()>& callback);
  void unsubscribe(const void* receptor);

  void notify();

 private:
  using ReceptorCallbacks = std::vector<ReceptorCallback>;
  ReceptorCallbacks _receptor_cbks;
};
}  // namespace pancake