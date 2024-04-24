#include "graphics/material.hpp"

using namespace pancake;

Material::Material(const GUID& guid) : _guid(guid) {
  setResourceGuid<MaterialResource, MaterialOrigin>(guid);
}

template <>
void Material::resourceUpdated<MaterialOrigin>(const MaterialResource& res) {
  _shader = res.getShader();
  _depth_test = res.getDepthTest();
  _stage = res.getStage();
  _light_pass_input_name = res.getLightPassInputName();
  _view_input_name = res.getViewInputName();
  _inputs = res.getInputs();
}

void Material::resourcesUpdated() {}

const GUID& Material::getShader() const {
  return _shader;
}

bool Material::getDepthTest() const {
  return _depth_test;
}

int Material::getStage() const {
  return _stage;
}

std::string_view Material::getLightPassInputName() const {
  return _light_pass_input_name;
}

std::string_view Material::getViewInputName() const {
  return _view_input_name;
}

const std::set<ShaderInput>& Material::getInputs() const {
  return _inputs;
}

const GUID& Material::guid() const {
  return _guid;
}