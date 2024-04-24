#pragma once

#include "graphics/vertex.hpp"
#include "resources/resource.hpp"

#include <map>
#include <span>
#include <vector>

namespace pancake {
class ObjResource : public Resource {
 public:
  ObjResource(std::string_view path, const GUID& guid);
  virtual ~ObjResource() = default;

  std::span<const Vertex> getVertices(std::string_view name) const;
  std::span<const unsigned int> getIndices(std::string_view name) const;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  std::map<std::string, std::vector<Vertex>, std::less<>> _vertices;
  std::map<std::string, std::vector<unsigned int>, std::less<>> _indices;
};
}  // namespace pancake