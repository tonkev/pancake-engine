#pragma once

#include "type_desc_library.hpp"

#include <functional>
#include <limits>

namespace pancake {
template <typename T>
struct GenerationalIndex {
 public:
  using underlying_type = T;

  GenerationalIndex() : id(std::numeric_limits<T>::max()), gen(std::numeric_limits<T>::max()) {}
  GenerationalIndex(T id, T gen) : id(id), gen(gen) {}

  GenerationalIndex(const GenerationalIndex<T>& other) = default;

  bool operator<(const GenerationalIndex<T>& rhs) const {
    return std::tie(id, gen) < std::tie(rhs.id, rhs.gen);
  }

  bool operator==(const GenerationalIndex<T>& rhs) const {
    return std::tie(id, gen) == std::tie(rhs.id, rhs.gen);
  }

  bool operator!=(const GenerationalIndex<T>& rhs) const {
    return std::tie(id, gen) != std::tie(rhs.id, rhs.gen);
  }

  static const GenerationalIndex<T> null;

  T id;
  T gen;

 private:
  static inline const TypeDesc& DESC = TypeDescLibrary::get<GenerationalIndex<T>>()
                                           .addField("id", 0, TypeDescLibrary::get<T>())
                                           .addField("gen", sizeof(T), TypeDescLibrary::get<T>());
};

template <typename T>
const GenerationalIndex<T> GenerationalIndex<T>::null(std::numeric_limits<T>::max(),
                                                      std::numeric_limits<T>::max());
}  // namespace pancake

template <typename T>
struct std::hash<pancake::GenerationalIndex<T>> {
  size_t operator()(const pancake::GenerationalIndex<T>& id) const noexcept {
    return std::hash<T>{}(id.id) | (std::hash<T>{}(id.gen) << 1);
  }
};