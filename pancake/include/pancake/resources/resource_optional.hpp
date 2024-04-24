#pragma once

#include "resources/resource.hpp"
#include "util/guid.hpp"

#include <functional>
#include <optional>

namespace pancake {
template <Resourceable T>
class ResourceOptional : public std::optional<std::reference_wrapper<T>> {
 private:
  using Parent = std::optional<std::reference_wrapper<T>>;

 public:
  using Parent::Parent;
  virtual ~ResourceOptional() = default;

  const GUID& guid() const {
    if (this->has_value()) {
      return this->value().get().guid();
    } else {
      return GUID::null;
    }
  }
};
}  // namespace pancake