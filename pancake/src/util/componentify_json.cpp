#include "util/componentify_json.hpp"

#include "util/array_type_desc.hpp"
#include "util/dynamic_buffer_type_desc.hpp"
#include "util/primitive_type_desc.hpp"
#include "util/struct_type_desc.hpp"

#include "util/fewi.hpp"

using namespace pancake;

ComponentifyJSON::ComponentifyJSON(void* data, const JSONValue& json) : _data(data), _json(json) {}

void ComponentifyJSON::operator()(const PrimitiveTypeDesc& desc) const {
  if (const JSONNumber* json_num = _json.asNumber(); (nullptr != json_num)) {
    if (desc.isSignedInteger()) {
      desc.setValue(_data, json_num->getInt64());
    } else if (desc.isUnsignedInteger()) {
      desc.setValue(_data, json_num->getUInt64());
    } else {
      desc.setValue(_data, json_num->getDouble());
    }
  }
}

void ComponentifyJSON::operator()(const ArrayTypeDesc& desc) const {
  if (const JSONArray* json_arr = _json.asArray(); (nullptr != json_arr)) {
    const TypeDesc& element_desc = desc.elementDesc();
    for (size_t i = 0; (i < desc.length()) && (i < json_arr->size()); ++i) {
      element_desc.visit(
          ComponentifyJSON(static_cast<char*>(_data) + (i * element_desc.size()), (*json_arr)[i]));
    }
  }
}

void ComponentifyJSON::operator()(const StructTypeDesc& desc) const {
  if (const JSONObject* json_obj = _json.asObject(); (nullptr != json_obj)) {
    for (const StructTypeDesc::Field& field : desc.getFields()) {
      if (const JSONValue* field_json = json_obj->get(field.name()); (nullptr != field_json)) {
        field.desc().visit(
            ComponentifyJSON(static_cast<char*>(_data) + field.offset(), *field_json));
      }
    }
  }
}

void ComponentifyJSON::operator()(const DynamicBufferTypeDesc& desc) const {
  if (const JSONArray* json_arr = _json.asArray(); (nullptr != json_arr)) {
    const TypeDesc& element_desc = desc.elementDesc();
    for (size_t i = 0; (i < json_arr->size()); ++i) {
      element_desc.visit(ComponentifyJSON(desc.addElement(_data), (*json_arr)[i]));
    }
  }
}
