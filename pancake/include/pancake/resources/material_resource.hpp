#pragma once

#include "resources/json_resource.hpp"

#include "graphics/shader_input.hpp"

#include <set>

namespace pancake {
class MaterialResource : public JSONResource {
 public:
  MaterialResource(std::string_view path, const GUID& guid);
  virtual ~MaterialResource() = default;

  void setShader(const GUID& shader);
  void setDepthTest(bool value);
  void setStage(int stage);
  void setLightPassInputName(std::string_view input_name);
  void setViewInputName(std::string_view input_name);

  void addInput(const ShaderInput& input);
  void removeInput(std::string_view input_name);

  const GUID& getShader() const;
  bool getDepthTest() const;
  int getStage() const;
  std::string_view getLightPassInputName() const;
  std::string_view getViewInputName() const;

  const std::set<ShaderInput>& getInputs() const;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  GUID _shader = GUID::null;
  bool _depth_test = true;
  int _stage = 10000;
  std::string _light_pass_input_name = "";
  std::string _view_input_name = "";
  std::set<ShaderInput> _inputs;
};
}  // namespace pancake