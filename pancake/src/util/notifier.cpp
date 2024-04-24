#include "util/notifier.hpp"

using namespace pancake;

void Notifier::subscribe(WeakPtr<void> receptor, const std::function<void()>& callback) {
  _receptor_cbks.emplace_back(receptor, callback);
}

void Notifier::unsubscribe(const void* receptor) {
  std::erase_if(_receptor_cbks, [&receptor](ReceptorCallback& rc) {
    if (Ptr<void> rc_receptor = rc.receptor.lock()) {
      return receptor == rc_receptor.get();
    } else {
      return true;  // expired ptr, delete whether or not it matches
    }
  });
}

void Notifier::notify() {
  // remove receptors that have been destroyed since subscription
  std::erase_if(_receptor_cbks, [](ReceptorCallback& rc) { return rc.receptor.expired(); });

  // copy made to avoid tripping up over iterators if
  // subscribe/unsubscribe are called via the callback
  ReceptorCallbacks tmp_receptor_cbks = _receptor_cbks;
  for (const ReceptorCallback& rc : tmp_receptor_cbks) {
    if (!rc.receptor.expired()) {
      rc.callback();
    }
  }
}