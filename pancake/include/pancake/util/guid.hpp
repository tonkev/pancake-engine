#pragma once

#include "util/type_desc.hpp"

#include <compare>
#include <cstdint>
#include <functional>
#include <string>

namespace pancake {
class GUID {
 public:
  GUID();
  GUID(const GUID&) = default;

  auto operator<=>(const GUID&) const = default;

  static GUID gen();
  static GUID fromHex(const std::string& hex_str);

  std::string hex() const;

  static const GUID null;

 private:
  uint64_t _id;

  static const TypeDesc& DESC;

  friend std::hash<GUID>;
};
}  // namespace pancake

template <>
struct std::hash<pancake::GUID> {
  size_t operator()(const pancake::GUID& guid) const noexcept {
    return std::hash<uint64_t>{}(guid._id);
  }
};