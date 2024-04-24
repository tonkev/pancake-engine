#pragma once

#include <memory>

namespace pancake {
template <typename T>
using Ptr = std::shared_ptr<T>;

template <typename T>
using WeakPtr = std::weak_ptr<T>;
}  // namespace pancake