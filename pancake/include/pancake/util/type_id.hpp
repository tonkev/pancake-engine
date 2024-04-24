#pragma once

#include <functional>
#include <type_traits>

namespace pancake {
class TypeId {
 public:
  TypeId(const TypeId&) = delete;

  bool operator==(const TypeId& rhs) const;

  template <typename T>
  static std::enable_if_t<!std::is_const_v<T>, TypeId&> get() {
    static TypeId id;
    return id;
  }

  template <typename T>
  static std::enable_if_t<std::is_const_v<T>, TypeId&> get() {
    return get<std::remove_const_t<T>>();
  }

 protected:
  TypeId() = default;
};
}  // namespace pancake

template <>
struct std::hash<pancake::TypeId> {
  size_t operator()(const pancake::TypeId& tid) const noexcept;
};