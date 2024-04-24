#include "resources/gl3_shader_resource.hpp"

#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/jsonify_component.hpp"

using namespace pancake;

Resources::Creator<GL3ShaderResource> gl3_shader_resource_creator;

const Resource::Type GL3ShaderResource::TYPE = Type::Gl3Shader;

GL3ShaderResource::GL3ShaderResource(std::string_view path, const GUID& guid)
    : JSONResource(path, guid) {}

void GL3ShaderResource::_load() {
  JSONResource::_load();
  if (const JSONValue* val = _json.get("vertex_source"); nullptr != val) {
    GUID guid = GUID::null;
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&guid, *val));
    setVertexSourceGuid(guid);
  }
  if (const JSONValue* val = _json.get("fragment_source"); nullptr != val) {
    GUID guid = GUID::null;
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&guid, *val));
    setFragmentSourceGuid(guid);
  }
}

void GL3ShaderResource::_save() {
  _json.clear();
  TypeDescLibrary::get<GUID>().visit(
      JSONifyComponent(_json, "vertex_source", &getVertexSourceGuid()));
  TypeDescLibrary::get<GUID>().visit(
      JSONifyComponent(_json, "fragment_source", &getFragmentSourceGuid()));
  JSONResource::_save();
}

void GL3ShaderResource::setVertexSourceGuid(const GUID& vertex_source) {
  setResourceGuid<TextResource, VertexSourceTag>(vertex_source);
  updated();
}

void GL3ShaderResource::setFragmentSourceGuid(const GUID& fragment_source) {
  setResourceGuid<TextResource, FragmentSourceTag>(fragment_source);
  updated();
}

const GUID& GL3ShaderResource::getVertexSourceGuid() const {
  return getResourceGuid<TextResource, VertexSourceTag>();
}

const GUID& GL3ShaderResource::getFragmentSourceGuid() const {
  return getResourceGuid<TextResource, FragmentSourceTag>();
}

template <>
void GL3ShaderResource::resourceUpdated<VertexSourceTag>(const TextResource& res) {
  _vertex_source = res.constText();
  updated();
}

template <>
void GL3ShaderResource::resourceUpdated<FragmentSourceTag>(const TextResource& res) {
  _fragment_source = res.constText();
  updated();
}

void GL3ShaderResource::resourcesUpdated() {}

void GL3ShaderResource::ensureUpdated(Resources& resources) {
  checkAndApplyResourceUpdates(resources);
}

std::string_view GL3ShaderResource::getVertexSource() const {
  return _vertex_source;
}

std::string_view GL3ShaderResource::getFragmentSource() const {
  return _fragment_source;
}

Resource& GL3ShaderResource::asResource() {
  return *this;
}

const Resource& GL3ShaderResource::asResource() const {
  return *this;
}

Resource::Type GL3ShaderResource::type() const {
  return Type::Gl3Shader;
}