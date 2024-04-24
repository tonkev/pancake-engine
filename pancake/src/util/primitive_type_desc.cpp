#include "util/primitive_type_desc.hpp"

#include "util/type_desc_visitor.hpp"

using namespace pancake;

PrimitiveTypeDesc& PrimitiveTypeDesc::setName(std::string_view name) {
  TypeDesc::setName(name);
  return *this;
}

void PrimitiveTypeDesc::visit(const TypeDescVisitor& visitor) const {
  visitor(*this);
}

bool PrimitiveTypeDesc::operator==(const PrimitiveTypeDesc& rhs) const {
  return this == &rhs;
}