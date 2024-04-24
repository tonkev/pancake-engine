#pragma once

#include "util/type_desc.hpp"

#include <vector>

namespace pancake {
class TypeDescLibrary;
class ArrayTypeDesc : public TypeDesc {
 public:
  ArrayTypeDesc(const ArrayTypeDesc&) = delete;

  ArrayTypeDesc& setName(std::string_view name);

  const TypeDesc& elementDesc() const;
  size_t length() const;

  virtual void visit(const TypeDescVisitor& visitor) const override;

  bool operator==(const ArrayTypeDesc& rhs) const;

 protected:
  ArrayTypeDesc(size_t size, const TypeDesc& element_desc, const void* default_value);

  friend TypeDescLibrary;

 private:
  const TypeDesc& _element_desc;
};
}  // namespace pancake