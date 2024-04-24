#pragma once

#include "resources/json_resource.hpp"
#include "resources/resource_user.hpp"
#include "resources/shader_resource_interface.hpp"

#include "util/matrix.hpp"

namespace pancake {
struct VertexSourceTag;
struct FragmentSourceTag;
class GL3ShaderResource
    : public JSONResource,
      public ShaderResourceInterface,
      public ResourceUser<
          GL3ShaderResource,
          ResourceUse<GL3ShaderResource, pancake::TextResource, VertexSourceTag>,
          ResourceUse<GL3ShaderResource, pancake::TextResource, FragmentSourceTag>> {
 public:
  GL3ShaderResource(std::string_view path, const GUID& guid);
  virtual ~GL3ShaderResource() = default;

  void setVertexSourceGuid(const GUID& vertex_source);
  void setFragmentSourceGuid(const GUID& fragment_source);

  const GUID& getVertexSourceGuid() const;
  const GUID& getFragmentSourceGuid() const;

  template <typename T>
  void resourceUpdated(const TextResource& res);
  void resourcesUpdated();

  virtual void ensureUpdated(Resources& resources) override;

  virtual std::string_view getVertexSource() const override;
  virtual std::string_view getFragmentSource() const override;

  virtual Resource& asResource() override;
  virtual const Resource& asResource() const override;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  std::string _vertex_source;
  std::string _fragment_source;
};
}  // namespace pancake