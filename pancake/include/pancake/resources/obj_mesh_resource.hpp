#pragma once

#include "resources/json_resource.hpp"
#include "resources/mesh_resource_interface.hpp"
#include "resources/obj_resource.hpp"
#include "resources/resource_user.hpp"

#include <vector>

namespace pancake {
struct ObjTag {};

class ObjMeshResource
    : public JSONResource,
      public MeshResourceInterface,
      public ResourceUser<ObjMeshResource,
                          ResourceUse<ObjMeshResource, pancake::ObjResource, ObjTag>> {
 public:
  ObjMeshResource(std::string_view path, const GUID& guid);
  virtual ~ObjMeshResource() = default;

  void setObj(const GUID& obj);
  void setName(std::string_view name);

  const GUID& getObj() const;
  std::string_view getName() const;

  template <typename T>
  void resourceUpdated(const ObjResource& res);
  void resourcesUpdated();

  virtual void ensureUpdated(Resources& resources) override;

  virtual std::span<const Vertex> getVertices() const override;
  virtual std::span<const unsigned int> getIndices() const override;

  virtual Resource& asResource() override;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  std::string _name;

  std::vector<Vertex> _vertices;
  std::vector<unsigned int> _indices;
};
}  // namespace pancake