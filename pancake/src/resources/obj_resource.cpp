#include "resources/obj_resource.hpp"

#include "resources/resources.hpp"
#include "util/fewi.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <sstream>

using namespace pancake;

Resources::Creator<ObjResource> obj_resource_creator;

const Resource::Type ObjResource::TYPE = Type::Obj;

ObjResource::ObjResource(std::string_view path, const GUID& guid) : Resource(path, guid) {}

void ObjResource::_load() {
  _vertices.clear();
  _indices.clear();

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> mats;
  std::string warn;
  std::string err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &mats, &warn, &err, std::string(fullPath()).c_str())) {
    FEWI::error() << err;
    return;
  }

  for (const auto& shape : shapes) {
    std::vector<Vertex>& vertices = _vertices[shape.name];
    std::vector<unsigned int>& indices = _indices[shape.name];

    vertices.reserve(shape.mesh.indices.size());
    indices.reserve(shape.mesh.indices.size());

    for (size_t i = 0; i < shape.mesh.indices.size(); ++i) {
      const auto& index = shape.mesh.indices[i];
      Vertex& vertex = vertices.emplace_back();

      vertex.position = Vec4f(attrib.vertices[3 * index.vertex_index + 0],
                              attrib.vertices[3 * index.vertex_index + 1],
                              attrib.vertices[3 * index.vertex_index + 2], 1.f);

      vertex.normal = Vec4f(attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2], 0.f);

      vertex.color = Vec4f(attrib.colors[3 * index.vertex_index + 0],
                           attrib.colors[3 * index.vertex_index + 1],
                           attrib.colors[3 * index.vertex_index + 2], 1.f);

      if (0 <= index.texcoord_index) {
        vertex.uv0 = Vec2f(attrib.texcoords[2 * index.texcoord_index + 0],
                           attrib.texcoords[2 * index.texcoord_index + 1]);
      }

      indices.push_back(i);
    }
  }
}

void ObjResource::_save() {}

std::span<const Vertex> ObjResource::getVertices(std::string_view name) const {
  if (const auto it = _vertices.find(name); it != _vertices.end()) {
    return it->second;
  }
  return {};
}

std::span<const unsigned int> ObjResource::getIndices(std::string_view name) const {
  if (const auto it = _indices.find(name); it != _indices.end()) {
    return it->second;
  }
  return {};
}

Resource::Type ObjResource::type() const {
  return Type::Obj;
}