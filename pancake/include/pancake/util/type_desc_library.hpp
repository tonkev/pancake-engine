#pragma once

#include "ecs/dynamic_buffer.hpp"
#include "util/array_type_desc.hpp"
#include "util/dynamic_buffer_type_desc.hpp"
#include "util/primitive_type_desc.hpp"
#include "util/struct_type_desc.hpp"
#include "util/type_desc.hpp"

#include <map>
#include <string>
#include <type_traits>

namespace pancake {
class TypeDescLibrary {
 public:
  TypeDescLibrary() = delete;

  template <typename T>
  static std::enable_if_t<!std::is_const_v<T> && !std::is_array_v<T> && !std::is_class_v<T>,
                          TypeDesc&>
  get() {
    static TypedPrimitiveTypeDesc<T> desc;
    return desc;
  }

  template <typename T>
  static std::enable_if_t<!std::is_const_v<T> && std::is_array_v<T> && !std::is_class_v<T>,
                          ArrayTypeDesc&>
  get() {
    using element_type = std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
    static T default_value;
    static ArrayTypeDesc desc(sizeof(T), TypeDescLibrary::get<element_type>(), &default_value);
    return desc;
  }

  template <typename T>
  static std::enable_if_t<!std::is_const_v<T> && !std::is_array_v<T> && std::is_class_v<T> &&
                              !IsDynamicBuffer<T>::value,
                          StructTypeDesc&>
  get() {
    static const T default_value;
    static StructTypeDesc desc(sizeof(T), &default_value);
    return desc;
  }

  template <typename T>
  static std::enable_if_t<IsDynamicBuffer<T>::value, TypedDynamicBufferTypeDesc<T>&> get() {
    using element_type = std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
    static const T default_value;
    static TypedDynamicBufferTypeDesc<T> desc(TypeDescLibrary::get<element_type>(), &default_value);
    return desc;
  }

  template <typename T>
  static std::enable_if_t<std::is_const_v<T>, TypeDesc&> get() {
    return get<std::remove_const_t<T>>();
  }

  static TypeDesc* get(std::string_view name);

 protected:
  static void set(std::string_view name, TypeDesc& desc);

  friend TypeDesc;

 private:
  using NameDescMap = std::map<std::string, std::reference_wrapper<TypeDesc>, std::less<>>;
  static NameDescMap& getNameDescMap();
};
}  // namespace pancake