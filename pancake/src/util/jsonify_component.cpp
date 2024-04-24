#include "util/jsonify_component.hpp"

#include "util/array_type_desc.hpp"
#include "util/dynamic_buffer_type_desc.hpp"
#include "util/primitive_type_desc.hpp"
#include "util/struct_type_desc.hpp"

using namespace pancake;

JSONifyComponent::JSONifyComponent(JSONObject& parent_json, std::string_view name, const void* data)
    : _parent_json(parent_json), _name(name), _data(data) {}

JSONifyComponent::JSONifyComponent(JSONArray& parent_json, const void* data)
    : _parent_json(parent_json), _data(data) {}

void JSONifyComponent::operator()(const PrimitiveTypeDesc& desc) const {
  if (desc.isSignedInteger()) {
    addJSONValue<JSONNumber>().set(desc.castToInt64(_data));
  } else if (desc.isUnsignedInteger()) {
    addJSONValue<JSONNumber>().set(desc.castToUInt64(_data));
  } else {
    addJSONValue<JSONNumber>().set(desc.castToDouble(_data));
  }
}

void JSONifyComponent::operator()(const ArrayTypeDesc& desc) const {
  JSONArray& json_arr = addJSONValue<JSONArray>();
  const TypeDesc& element_desc = desc.elementDesc();
  for (size_t i = 0; i < desc.length(); ++i) {
    element_desc.visit(
        JSONifyComponent(json_arr, static_cast<const char*>(_data) + (i * element_desc.size())));
  }
}

void JSONifyComponent::operator()(const StructTypeDesc& desc) const {
  JSONObject& json_obj = addJSONValue<JSONObject>();
  for (const StructTypeDesc::Field& field : desc.getFields()) {
    field.desc().visit(
        JSONifyComponent(json_obj, field.name(), static_cast<const char*>(_data) + field.offset()));
  }
}

void JSONifyComponent::operator()(const DynamicBufferTypeDesc& desc) const {
  JSONArray& json_arr = addJSONValue<JSONArray>();
  const TypeDesc& element_desc = desc.elementDesc();
  for (size_t i = 0; i < desc.length(_data); ++i) {
    element_desc.visit(JSONifyComponent(json_arr, desc.getElement(_data, i)));
  }
}
