#pragma once

#include "ecs/dynamic_buffer.hpp"
#include "util/type_desc.hpp"

namespace pancake {
class TypeDescLibrary;
class DynamicBufferTypeDesc : public TypeDesc {
 public:
  DynamicBufferTypeDesc(size_t size, const TypeDesc& element_desc, const void* default_value);

  const TypeDesc& elementDesc() const;

  virtual size_t length(const void* buffer) const = 0;

  virtual void* getElement(void* buffer, int index) const = 0;
  virtual const void* getElement(const void* buffer, int index) const = 0;

  virtual void* addElement(void* buffer) const = 0;
  virtual void addElement(void* buffer, const void* element) const = 0;
  virtual void setElement(void* buffer, const void* element, int index) const = 0;

  virtual bool requiresDestroy() const override;

  virtual void visit(const TypeDescVisitor& visitor) const override;

  bool operator==(const DynamicBufferTypeDesc& rhs) const;

 private:
  const TypeDesc& _element_desc;
};

template <DynamicBufferable T>
class TypedDynamicBufferTypeDesc : public DynamicBufferTypeDesc {
 public:
  TypedDynamicBufferTypeDesc(const DynamicBufferTypeDesc&) = delete;

  virtual size_t length(const void* buffer) const override {
    return static_cast<const T*>(buffer)->size();
  }

  virtual void* getElement(void* buffer, int index) const override {
    return &(*static_cast<T*>(buffer))[index];
  }

  virtual const void* getElement(const void* buffer, int index) const override {
    return &(*static_cast<const T*>(buffer))[index];
  }

  virtual void* addElement(void* buffer) const override {
    return &(static_cast<T*>(buffer)->emplaceBack());
  }

  virtual void addElement(void* buffer, const void* element) const override {
    static_cast<T*>(buffer)->emplaceBack() = *static_cast<const T::element_type*>(element);
  }

  virtual void setElement(void* buffer, const void* element, int index) const override {
    (*static_cast<T*>(buffer))[index] = *static_cast<const T::element_type*>(element);
  }

  TypedDynamicBufferTypeDesc& setName(std::string_view name) {
    TypeDesc::setName(name);
    return *this;
  }

  virtual void destroy(void* data) const override { static_cast<T*>(data)->destroy(); }

 protected:
  TypedDynamicBufferTypeDesc(const TypeDesc& element_desc, const void* default_value)
      : DynamicBufferTypeDesc(sizeof(T), element_desc, default_value) {}

  friend TypeDescLibrary;
};
}  // namespace pancake