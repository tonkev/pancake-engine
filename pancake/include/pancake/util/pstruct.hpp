#pragma once

#include "util/type_desc_library.hpp"

#define PSTRUCT(name)                                                                              \
  struct name {                                                                                    \
    static pancake::StructTypeDesc& desc() {                                                       \
      static pancake::StructTypeDesc& desc = pancake::TypeDescLibrary::get<name>().setName(#name); \
      return desc;                                                                                 \
    }                                                                                              \
    static const name& default_value() {                                                           \
      return *static_cast<const name*>(desc().default_value());                                    \
    }                                                                                              \
    static inline const pancake::StructTypeDesc& DESC = desc();

#define PSTRUCT_MEMBER(member_type, member_name)                                                   \
  member_type member_name;                                                                         \
  static inline const void* STATIC_ADDER_##member_name = [](pancake::StructTypeDesc& desc,         \
                                                            const auto& instance) -> const void* { \
    desc.addField(#member_name, size_t(&(instance.member_name)) - size_t(&instance),               \
                  pancake::TypeDescLibrary::get<member_type>());                                   \
    return nullptr;                                                                                \
  }(desc(), default_value());

#define PSTRUCT_MEMBER_INITIALISED(member_type, member_name, initialiser)                          \
  member_type member_name = initialiser;                                                           \
  static inline const void* STATIC_ADDER_##member_name = [](pancake::StructTypeDesc& desc,         \
                                                            const auto& instance) -> const void* { \
    desc.addField(#member_name, size_t(&(instance.member_name)) - size_t(&instance),               \
                  pancake::TypeDescLibrary::get<member_type>());                                   \
    return nullptr;                                                                                \
  }(desc(), default_value());

#define PSTRUCT_END() \
  }                   \
  ;
