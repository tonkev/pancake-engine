#pragma once

#include "util/json.hpp"
#include "util/type_desc_visitor.hpp"

#include <variant>

namespace pancake {
class JSONifyComponent : public TypeDescVisitor {
 public:
  JSONifyComponent(JSONObject& parent_json, std::string_view name, const void* data);
  JSONifyComponent(JSONArray& parent_json, const void* data);

  virtual void operator()(const PrimitiveTypeDesc& desc) const override;
  virtual void operator()(const ArrayTypeDesc& desc) const override;
  virtual void operator()(const StructTypeDesc& desc) const override;
  virtual void operator()(const DynamicBufferTypeDesc& desc) const override;

  template <JSONValuable T>
  T& addJSONValue() const {
    if (std::holds_alternative<std::reference_wrapper<JSONObject>>(_parent_json)) {
      return std::get<std::reference_wrapper<JSONObject>>(_parent_json).get().getOrCreate<T>(_name);
    } else {
      return std::get<std::reference_wrapper<JSONArray>>(_parent_json).get().add<T>();
    }
  }

 private:
  std::variant<std::reference_wrapper<JSONObject>, std::reference_wrapper<JSONArray>> _parent_json;
  std::string_view _name;
  const void* _data;
};
}  // namespace pancake