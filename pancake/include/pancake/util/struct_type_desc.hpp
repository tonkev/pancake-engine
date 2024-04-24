#pragma once

#include "util/type_desc.hpp"

#include <vector>

namespace pancake {
class TypeDescLibrary;
class StructTypeDesc : public TypeDesc {
 public:
  class Field {
   public:
    Field(std::string_view name, size_t offset, const TypeDesc& desc);

    std::string_view name() const;
    size_t offset() const;
    const TypeDesc& desc() const;

   private:
    std::string _name;
    size_t _offset;
    const TypeDesc& _desc;
  };

  using Fields = std::vector<Field>;

  StructTypeDesc(const StructTypeDesc&) = delete;

  StructTypeDesc& setName(std::string_view name);
  StructTypeDesc& addField(std::string_view name, size_t offset, const TypeDesc& desc);

  const Fields& getFields() const;

  virtual void visit(const TypeDescVisitor& visitor) const override;

  bool operator==(const StructTypeDesc& rhs) const;

 protected:
  using TypeDesc::TypeDesc;

  friend TypeDescLibrary;

 private:
  Fields _fields;
};
}  // namespace pancake