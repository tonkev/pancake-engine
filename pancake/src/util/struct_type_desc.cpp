#include "util/struct_type_desc.hpp"

#include "util/type_desc_visitor.hpp"

using namespace pancake;

StructTypeDesc::Field::Field(std::string_view name, size_t offset, const TypeDesc& desc)
    : _name(name), _offset(offset), _desc(desc) {}

std::string_view StructTypeDesc::Field::name() const {
  return _name;
}

size_t StructTypeDesc::Field::offset() const {
  return _offset;
}

const TypeDesc& StructTypeDesc::Field::desc() const {
  return _desc;
}

StructTypeDesc& StructTypeDesc::setName(std::string_view name) {
  TypeDesc::setName(name);
  return *this;
}

StructTypeDesc& StructTypeDesc::addField(std::string_view name,
                                         size_t offset,
                                         const TypeDesc& desc) {
  _fields.emplace_back(name, offset, desc);
  return *this;
}

const StructTypeDesc::Fields& StructTypeDesc::getFields() const {
  return _fields;
}

void StructTypeDesc::visit(const TypeDescVisitor& visitor) const {
  visitor(*this);
}

bool StructTypeDesc::operator==(const StructTypeDesc& rhs) const {
  return this == &rhs;
}