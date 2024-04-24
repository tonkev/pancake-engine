#pragma once

#include <functional>
#include <string>

namespace pancake {
class TypeDescLibrary;
class TypeDescVisitor;
class TypeDesc {
 public:
  TypeDesc(const TypeDesc&) = delete;

  TypeDesc& setName(std::string_view name);

  std::string_view name() const;
  size_t size() const;
  const void* default_value() const;

  virtual bool requiresDestroy() const;
  virtual void destroy(void* data) const;

  virtual void visit(const TypeDescVisitor& visitor) const = 0;

  bool operator==(const TypeDesc& rhs) const;

 protected:
  TypeDesc(size_t size, const void* default_value);

  friend TypeDescLibrary;

  std::string _name;
  size_t _size;
  const void* _default_value;
};
}  // namespace pancake

template <>
struct std::hash<pancake::TypeDesc> {
  size_t operator()(const pancake::TypeDesc& desc) const noexcept;
};