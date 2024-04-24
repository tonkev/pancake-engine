#include "resources/material_resource.hpp"

#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/jsonify_component.hpp"

using namespace pancake;

Resources::Creator<MaterialResource> material_resource_creator;

const Resource::Type MaterialResource::TYPE = Type::Material;

MaterialResource::MaterialResource(std::string_view path, const GUID& guid)
    : JSONResource(path, guid) {}

void MaterialResource::_load() {
  _inputs.clear();
  JSONResource::_load();
  if (const JSONValue* val = _json.get("shader"); nullptr != val) {
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&_shader, *val));
  }
  if (const JSONValue* val = _json.get("depth_test"); nullptr != val) {
    TypeDescLibrary::get<bool>().visit(ComponentifyJSON(&_depth_test, *val));
  }
  if (const JSONValue* val = _json.get("stage"); nullptr != val) {
    TypeDescLibrary::get<int>().visit(ComponentifyJSON(&_stage, *val));
  }
  if (const JSONString* val = _json.get<JSONString>("light_pass_input_name"); nullptr != val) {
    _light_pass_input_name = *val;
  }
  if (const JSONString* val = _json.get<JSONString>("view_input_name"); nullptr != val) {
    _view_input_name = *val;
  }
  if (const JSONObject* obj = _json.get<JSONObject>("inputs"); nullptr != obj) {
    for (const auto& [name, input_val] : obj->pairs()) {
      if (const JSONObject* input_obj = input_val->asObject(); nullptr != input_obj) {
        if (const std::optional<ShaderInput> input = ShaderInput::fromJson(*input_obj);
            input.has_value()) {
          _inputs.emplace(input.value());
        }
      }
    }
  }
}

void MaterialResource::_save() {
  _json.clear();
  TypeDescLibrary::get<GUID>().visit(JSONifyComponent(_json, "shader", &_shader));
  TypeDescLibrary::get<bool>().visit(JSONifyComponent(_json, "depth_test", &_depth_test));
  TypeDescLibrary::get<int>().visit(JSONifyComponent(_json, "stage", &_stage));
  _json.getOrCreate<JSONString>("light_pass_input_name") += _light_pass_input_name;
  _json.getOrCreate<JSONString>("view_input_name") += _view_input_name;

  if (!_inputs.empty()) {
    JSONArray& inputs_arr = _json.getOrCreate<JSONArray>("inputs");
    for (const ShaderInput& input : _inputs) {
      JSONObject& input_obj = inputs_arr.add<JSONObject>();
      input.toJson(input_obj);
    }
  }

  JSONResource::_save();
}

void MaterialResource::setShader(const GUID& shader) {
  _shader = shader;
  updated();
}

void MaterialResource::setDepthTest(bool value) {
  _depth_test = value;
  updated();
}
void MaterialResource::setStage(int stage) {
  _stage = stage;
  updated();
}

void MaterialResource::setLightPassInputName(std::string_view input_name) {
  _light_pass_input_name = input_name;
  updated();
}

void MaterialResource::setViewInputName(std::string_view input_name) {
  _view_input_name = input_name;
  updated();
}

void MaterialResource::addInput(const ShaderInput& input) {
  _inputs.emplace(input);
}

void MaterialResource::removeInput(std::string_view input_name) {
  std::erase_if(_inputs,
                [&input_name](const ShaderInput& input) { return input.getName() == input_name; });
}

const GUID& MaterialResource::getShader() const {
  return _shader;
}

bool MaterialResource::getDepthTest() const {
  return _depth_test;
}

int MaterialResource::getStage() const {
  return _stage;
}

std::string_view MaterialResource::getLightPassInputName() const {
  return _light_pass_input_name;
}

std::string_view MaterialResource::getViewInputName() const {
  return _view_input_name;
}

const std::set<ShaderInput>& MaterialResource::getInputs() const {
  return _inputs;
}

Resource::Type MaterialResource::type() const {
  return Type::Material;
}