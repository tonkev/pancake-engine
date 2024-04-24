#include "util/type_desc.hpp"

#include "util/type_desc_library.hpp"
#include "util/type_desc_visitor.hpp"

using namespace pancake;

TypeDesc::TypeDesc(size_t size, const void* default_value)
    : _name("<unset>"), _size(size), _default_value(default_value) {}

TypeDesc& TypeDesc::setName(std::string_view name) {
  _name = name;
  TypeDescLibrary::set(name, *this);
  return *this;
}

std::string_view TypeDesc::name() const {
  return _name;
}

size_t TypeDesc::size() const {
  return _size;
}

const void* TypeDesc::default_value() const {
  return _default_value;
}

bool TypeDesc::requiresDestroy() const {
  return false;
}

void TypeDesc::destroy(void* data) const {
  // do nothing
}

bool TypeDesc::operator==(const TypeDesc& rhs) const {
  return this == &rhs;
}

size_t std::hash<pancake::TypeDesc>::operator()(const pancake::TypeDesc& desc) const noexcept {
  return std::hash<const void*>{}(&desc);
};