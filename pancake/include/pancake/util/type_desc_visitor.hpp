#pragma once

namespace pancake {
class ArrayTypeDesc;
class PrimitiveTypeDesc;
class StructTypeDesc;
class DynamicBufferTypeDesc;
class TypeDescVisitor {
 public:
  virtual void operator()(const PrimitiveTypeDesc& desc) const = 0;
  virtual void operator()(const ArrayTypeDesc& desc) const = 0;
  virtual void operator()(const StructTypeDesc& desc) const = 0;
  virtual void operator()(const DynamicBufferTypeDesc& desc) const = 0;
};
}  // namespace pancake
