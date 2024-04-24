#pragma once

#include "util/type_desc.hpp"

#include <cstdint>
#include <vector>

namespace pancake {
class TypeDescLibrary;
class PrimitiveTypeDesc : public TypeDesc {
 public:
  PrimitiveTypeDesc(const PrimitiveTypeDesc&) = delete;

  PrimitiveTypeDesc& setName(std::string_view name);

  virtual void setValue(void* data, int64_t value) const = 0;
  virtual void setValue(void* data, uint64_t value) const = 0;
  virtual void setValue(void* data, double value) const = 0;

  virtual int64_t castToInt64(const void* data) const = 0;
  virtual uint64_t castToUInt64(const void* data) const = 0;
  virtual double castToDouble(const void* data) const = 0;

  virtual bool isSignedInteger() const = 0;
  virtual bool isUnsignedInteger() const = 0;
  virtual bool isFloatingPoint() const = 0;

  virtual void visit(const TypeDescVisitor& visitor) const override;

  bool operator==(const PrimitiveTypeDesc& rhs) const;

 protected:
  using TypeDesc::TypeDesc;

  friend TypeDescLibrary;
};

template <typename T>
class TypedPrimitiveTypeDesc : public PrimitiveTypeDesc {
 public:
  TypedPrimitiveTypeDesc() : PrimitiveTypeDesc(sizeof(T), &default_value) {}

  virtual void setValue(void* data, int64_t setValue) const override {
    *static_cast<T*>(data) = static_cast<T>(setValue);
  }

  virtual void setValue(void* data, uint64_t setValue) const override {
    *static_cast<T*>(data) = static_cast<T>(setValue);
  }

  virtual void setValue(void* data, double setValue) const override {
    *static_cast<T*>(data) = static_cast<T>(setValue);
  }

  virtual int64_t castToInt64(const void* data) const override {
    return static_cast<int64_t>(*static_cast<const T*>(data));
  }

  virtual uint64_t castToUInt64(const void* data) const override {
    return static_cast<uint64_t>(*static_cast<const T*>(data));
  }

  virtual double castToDouble(const void* data) const override {
    return static_cast<double>(*static_cast<const T*>(data));
  }

  virtual bool isSignedInteger() const override {
    return std::is_signed_v<T> && std::is_integral_v<T>;
  };

  virtual bool isUnsignedInteger() const override {
    return std::is_unsigned_v<T> && std::is_integral_v<T>;
  };

  virtual bool isFloatingPoint() const override { return std::is_floating_point_v<T>; }

 private:
  static inline const T default_value{};
};
}  // namespace pancake