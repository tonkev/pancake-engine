#pragma once

#include <functional>

namespace pancake {
template <typename T>
struct HashRefWrapperByAddr {
  size_t operator()(const std::reference_wrapper<T>& item) const {
    return std::hash<const void*>{}(&(item.get()));
  }
};

template <typename T>
struct EqualToRefWrapperByAddr {
  bool operator()(const std::reference_wrapper<T>& lhs,
                  const std::reference_wrapper<T>& rhs) const {
    return (&(lhs.get())) == (&(rhs.get()));
  }
};
};  // namespace pancake