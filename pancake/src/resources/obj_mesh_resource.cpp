#include "resources/obj_mesh_resource.hpp"

#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/jsonify_component.hpp"

using namespace pancake;

Resources::Creator<ObjMeshResource> obj_mesh_resource_creator;

const Resource::Type ObjMeshResource::TYPE = Type::ObjMesh;

ObjMeshResource::ObjMeshResource(std::string_view path, const GUID& guid)
    : JSONResource(path, guid) {}

void ObjMeshResource::_load() {
  JSONResource::_load();
  if (const JSONValue* val = _json.get("obj"); nullptr != val) {
    GUID obj;
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&obj, *val));
    setResourceGuid<ObjResource, ObjTag>(obj);
  }
  if (const JSONValue* val = _json.get("name"); nullptr != val) {
    if (const JSONString* str = val->asString(); nullptr != str) {
      _name = *str;
    }
  }
}

void ObjMeshResource::_save() {
  _json.clear();

  TypeDescLibrary::get<GUID>().visit(JSONifyComponent(_json, "obj", &getObj()));

  JSONString* str = new JSONString();
  *str += _name;

  JSONResource::_save();
}

void ObjMeshResource::setObj(const GUID& obj) {
  setResourceGuid<ObjResource, ObjTag>(obj);
  updated();
}

void ObjMeshResource::setName(std::string_view name) {
  _name = name;
  updated();
}

const GUID& ObjMeshResource::getObj() const {
  return getResourceGuid<ObjResource, ObjTag>();
}

std::string_view ObjMeshResource::getName() const {
  return _name;
}

template <>
void ObjMeshResource::resourceUpdated<ObjTag>(const ObjResource& res) {
  _vertices.clear();
  _indices.clear();

  std::span<const Vertex> vertices = res.getVertices(_name);
  _vertices.insert(_vertices.begin(), vertices.begin(), vertices.end());

  std::span<const unsigned int> indices = res.getIndices(_name);
  _indices.insert(_indices.begin(), indices.begin(), indices.end());

  updated();
}

void ObjMeshResource::resourcesUpdated() {}

void ObjMeshResource::ensureUpdated(Resources& resources) {
  checkAndApplyResourceUpdates(resources);
}

std::span<const Vertex> ObjMeshResource::getVertices() const {
  return _vertices;
}

std::span<const unsigned int> ObjMeshResource::getIndices() const {
  return _indices;
}

Resource& ObjMeshResource::asResource() {
  return *this;
}

Resource::Type ObjMeshResource::type() const {
  return Type::ObjMesh;
}