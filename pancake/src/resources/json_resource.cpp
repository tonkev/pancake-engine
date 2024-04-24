#include "resources/json_resource.hpp"

#include "resources/resources.hpp"
#include "util/json.hpp"

#include <sstream>

using namespace pancake;

Resources::Creator<JSONResource> json_resource_creator;

const Resource::Type JSONResource::TYPE = Type::Json;

JSONResource::JSONResource(std::string_view path, const GUID& guid)
    : TextResource(path, guid), _json() {}

void JSONResource::_load() {
  TextResource::_load();
  std::stringstream stream(_text);
  _json = JSONObject(stream);
}

void JSONResource::_save() {
  std::stringstream stream;
  _json.serialise(stream);
  _text = stream.str();
  TextResource::_save();
}

JSONObject& JSONResource::json() {
  updated();
  return _json;
}

const JSONObject& JSONResource::constJson() const {
  return _json;
}

Resource::Type JSONResource::type() const {
  return Type::Json;
}