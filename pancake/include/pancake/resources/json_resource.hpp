#pragma once

#include "resources/text_resource.hpp"

#include "util/json.hpp"

namespace pancake {
class JSONResource : public TextResource {
 public:
  JSONResource(std::string_view path, const GUID& guid);
  virtual ~JSONResource() = default;

  virtual Type type() const override;

  JSONObject& json();
  const JSONObject& constJson() const;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

  JSONObject _json;
};
}  // namespace pancake