#pragma once

#include "util/guid.hpp"

namespace pancake {
class Encompasser {
 public:
  Encompasser(const GUID& guid);
  virtual ~Encompasser() = default;

  const GUID& guid() const;

 private:
  GUID _guid;
};
}  // namespace pancake