#include "util/dynamic_buffer_type_desc.hpp"

#include "util/type_desc_visitor.hpp"

using namespace pancake;

DynamicBufferTypeDesc::DynamicBufferTypeDesc(size_t size,
                                             const TypeDesc& element_desc,
                                             const void* default_value)
    : TypeDesc(size, default_value), _element_desc(element_desc) {}

const TypeDesc& DynamicBufferTypeDesc::elementDesc() const {
  return _element_desc;
}

bool DynamicBufferTypeDesc::requiresDestroy() const {
  return true;
}

void DynamicBufferTypeDesc::visit(const TypeDescVisitor& visitor) const {
  visitor(*this);
}

bool DynamicBufferTypeDesc::operator==(const DynamicBufferTypeDesc& rhs) const {
  return this == &rhs;
}