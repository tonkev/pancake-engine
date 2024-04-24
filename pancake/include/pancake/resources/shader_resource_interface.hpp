#pragma once

#include "graphics/vertex.hpp"
#include "resources/resource.hpp"

#include <string>

namespace pancake {
class ShaderResourceInterface {
 public:
  ShaderResourceInterface() = default;
  virtual ~ShaderResourceInterface() = default;

  virtual std::string_view getVertexSource() const = 0;
  virtual std::string_view getFragmentSource() const = 0;

  virtual Resource& asResource() = 0;
  virtual const Resource& asResource() const = 0;
};
}  // namespace pancake