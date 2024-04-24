#pragma once

#include "util/json.hpp"
#include "util/type_desc_visitor.hpp"

#include <variant>

namespace pancake {
class ComponentifyJSON : public TypeDescVisitor {
 public:
  ComponentifyJSON(void* data, const JSONValue& json);

  virtual void operator()(const PrimitiveTypeDesc& desc) const override;
  virtual void operator()(const ArrayTypeDesc& desc) const override;
  virtual void operator()(const StructTypeDesc& desc) const override;
  virtual void operator()(const DynamicBufferTypeDesc& desc) const override;

 private:
  void* _data;
  const JSONValue& _json;
};
}  // namespace pancake