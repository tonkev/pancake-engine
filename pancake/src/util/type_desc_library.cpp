#include "util/type_desc_library.hpp"

using namespace pancake;

TypeDescLibrary::NameDescMap& TypeDescLibrary::getNameDescMap() {
  static NameDescMap map;
  return map;
}

void TypeDescLibrary::set(std::string_view name, TypeDesc& desc) {
  getNameDescMap().emplace(name, desc);
}

TypeDesc* TypeDescLibrary::get(std::string_view name) {
  const NameDescMap& name_descs = getNameDescMap();
  if (const auto it = name_descs.find(name); it != name_descs.end()) {
    return &it->second.get();
  }
  return nullptr;
}