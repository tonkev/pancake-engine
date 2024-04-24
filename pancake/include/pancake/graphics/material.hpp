#pragma once

#include "resources/material_resource.hpp"
#include "resources/resource_user.hpp"
#include "util/guid.hpp"

namespace pancake {
class Renderer;

struct MaterialOrigin {};

class Material
    : public ResourceUser<Material, ResourceUse<Material, MaterialResource, MaterialOrigin>> {
 public:
  Material(const GUID& guid);
  virtual ~Material() = default;

  template <typename T>
  void resourceUpdated(const MaterialResource& res);

  void resourcesUpdated();

  const GUID& getShader() const;
  bool getDepthTest() const;
  int getStage() const;
  std::string_view getLightPassInputName() const;
  std::string_view getViewInputName() const;
  const std::set<ShaderInput>& getInputs() const;

  const GUID& guid() const;

 private:
  GUID _guid = GUID::null;
  GUID _shader = GUID::null;
  bool _depth_test = true;
  int _stage = -1;
  std::string _light_pass_input_name = "";
  std::string _view_input_name = "";
  std::set<ShaderInput> _inputs{};
};
}  // namespace pancake