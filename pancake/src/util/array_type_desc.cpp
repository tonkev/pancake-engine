#include "util/array_type_desc.hpp"

#include "util/type_desc_visitor.hpp"

using namespace pancake;

ArrayTypeDesc::ArrayTypeDesc(size_t size, const TypeDesc& element_desc, const void* default_value)
    : TypeDesc(size, default_value), _element_desc(element_desc) {}

ArrayTypeDesc& ArrayTypeDesc::setName(std::string_view name) {
  TypeDesc::setName(name);
  return *this;
}

const TypeDesc& ArrayTypeDesc::elementDesc() const {
  return _element_desc;
}

size_t ArrayTypeDesc::length() const {
  return size() / _element_desc.size();
}

void ArrayTypeDesc::visit(const TypeDescVisitor& visitor) const {
  visitor(*this);
}

bool ArrayTypeDesc::operator==(const ArrayTypeDesc& rhs) const {
  return this == &rhs;
}