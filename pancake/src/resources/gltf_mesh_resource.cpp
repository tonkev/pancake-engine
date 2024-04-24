#include "resources/gltf_mesh_resource.hpp"

#include "resources/gltf_resource.hpp"
#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/fewi.hpp"
#include "util/jsonify_component.hpp"

#include "tinygltf/tiny_gltf.h"

using namespace pancake;

Resources::Creator<GltfMeshResource> gltf_mesh_resource_creator;

const Resource::Type GltfMeshResource::TYPE = Type::GltfMesh;

GltfMeshResource::GltfMeshResource(std::string_view path, const GUID& guid)
    : JSONResource(path, guid) {}

void GltfMeshResource::_load() {
  JSONResource::_load();
  if (const JSONValue* val = _json.get("gltf"); nullptr != val) {
    GUID gltf;
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&gltf, *val));
    setResourceGuid<GltfResource, GltfTag>(gltf);
  }
  if (const JSONValue* val = _json.get("id"); nullptr != val) {
    if (const JSONNumber* num = val->asNumber(); nullptr != num) {
      _id = static_cast<int>(num->getInt64());
    }
  }
}

void GltfMeshResource::_save() {
  _json.clear();

  TypeDescLibrary::get<GUID>().visit(JSONifyComponent(_json, "obj", &getGltf()));
  _json.getOrCreate<JSONNumber>("id").set(static_cast<int64_t>(_id));

  JSONResource::_save();
}

void GltfMeshResource::setGltf(const GUID& gltf) {
  setResourceGuid<GltfResource, GltfTag>(gltf);
  updated();
}

void GltfMeshResource::setId(int id) {
  _id = id;
  updated();
}

const pancake::GUID& GltfMeshResource::getGltf() const {
  return getResourceGuid<GltfResource, GltfTag>();
}

int GltfMeshResource::getId() const {
  return _id;
}

template <>
void GltfMeshResource::resourceUpdated<GltfTag>(const GltfResource& res) {
  _vertices.clear();
  _indices.clear();

  bool success = true;

  const auto is_idx_in_range = [&success](int idx, int size) {
    if ((idx < 0) || (size <= idx)) {
      FEWI::error() << "Index out of range!";
      success = false;
      return false;
    }
    return true;
  };

  const auto& model = res.getModel();
  if (static_cast<size_t>(_id) < model.meshes.size()) {
    const auto& mesh = model.meshes[_id];

    for (const auto& primitive : mesh.primitives) {
      if (primitive.mode != 4) {
        FEWI::error() << "Unsupported primitive mode (" << primitive.mode << ") !";
        success = false;
        continue;
      }

      int position_attrib = -1;
      if (const auto it = primitive.attributes.find("POSITION"); it != primitive.attributes.end()) {
        position_attrib = it->second;
      }

      if (!is_idx_in_range(position_attrib, model.accessors.size())) {
        continue;
      }

      const auto& position_accessor = model.accessors[position_attrib];

      if (!is_idx_in_range(position_accessor.bufferView, model.bufferViews.size())) {
        continue;
      }

      if (position_accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        FEWI::error() << "Accessor component type is not float!";
        success = false;
        continue;
      }

      if (position_accessor.type != TINYGLTF_TYPE_VEC3) {
        FEWI::error() << "Accessor type is not vec3!";
        success = false;
        continue;
      }

      const auto& position_buffer_view = model.bufferViews[position_accessor.bufferView];

      if (!is_idx_in_range(position_buffer_view.buffer, model.buffers.size())) {
        continue;
      }

      const auto& position_buffer = model.buffers[position_buffer_view.buffer];

      const int vertices_start = _vertices.size();
      _vertices.resize(_vertices.size() + position_accessor.count);

      const size_t position_stride =
          (0 < position_buffer_view.byteStride) ? position_buffer_view.byteStride : sizeof(Vec3f);

      for (size_t i = 0; i < position_accessor.count; ++i) {
        Vertex& vertex = _vertices[vertices_start + i];
        size_t idx =
            position_accessor.byteOffset + position_buffer_view.byteOffset + (i * position_stride);

        if (!is_idx_in_range(idx, position_buffer.data.size())) {
          _vertices.resize(vertices_start);
          continue;
        }

        vertex.position = Vec4f(*reinterpret_cast<const Vec3f*>(&(position_buffer.data[idx])), 0.f);
      }

      int normal_attrib = -1;
      if (const auto it = primitive.attributes.find("NORMAL"); it != primitive.attributes.end()) {
        normal_attrib = it->second;
      }

      if (!is_idx_in_range(normal_attrib, model.accessors.size())) {
        continue;
      }

      const auto& normal_accessor = model.accessors[normal_attrib];

      if (!is_idx_in_range(normal_accessor.bufferView, model.bufferViews.size())) {
        continue;
      }

      if (normal_accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        FEWI::error() << "Accessor component type is not float!";
        success = false;
        continue;
      }

      if (normal_accessor.type != TINYGLTF_TYPE_VEC3) {
        FEWI::error() << "Accessor type is not vec3!";
        success = false;
        continue;
      }

      const auto& normal_buffer_view = model.bufferViews[normal_accessor.bufferView];

      if (!is_idx_in_range(normal_buffer_view.buffer, model.buffers.size())) {
        continue;
      }

      const auto& normal_buffer = model.buffers[normal_buffer_view.buffer];

      if (position_accessor.count < normal_accessor.count) {
        _vertices.resize(_vertices.size() + normal_accessor.count);
      }

      const size_t normal_stride =
          (0 < normal_buffer_view.byteStride) ? normal_buffer_view.byteStride : sizeof(Vec3f);

      for (size_t i = 0; i < normal_accessor.count; ++i) {
        Vertex& vertex = _vertices[vertices_start + i];
        size_t idx =
            normal_accessor.byteOffset + normal_buffer_view.byteOffset + (i * normal_stride);

        if (!is_idx_in_range(idx, normal_buffer.data.size())) {
          _vertices.resize(vertices_start);
          continue;
        }

        vertex.normal = Vec4f(*reinterpret_cast<const Vec3f*>(&(normal_buffer.data[idx])), 0.f);
      }

      if (!is_idx_in_range(primitive.indices, model.accessors.size())) {
        continue;
      }

      const auto& indices_accessor = model.accessors[primitive.indices];

      if (!is_idx_in_range(indices_accessor.bufferView, model.bufferViews.size())) {
        continue;
      }

      if (indices_accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        FEWI::error() << "Accessor component type is not unsigned short!";
        success = false;
        continue;
      }

      if (indices_accessor.type != TINYGLTF_TYPE_SCALAR) {
        FEWI::error() << "Accessor type is not scalar!";
        success = false;
        continue;
      }

      const auto& indices_buffer_view = model.bufferViews[indices_accessor.bufferView];

      if (!is_idx_in_range(indices_buffer_view.buffer, model.buffers.size())) {
        continue;
      }

      const auto& indices_buffer = model.buffers[indices_buffer_view.buffer];

      const int indices_start = _indices.size();
      _indices.resize(_indices.size() + indices_accessor.count);

      const size_t indices_stride = (0 < indices_buffer_view.byteStride)
                                        ? indices_buffer_view.byteStride
                                        : sizeof(unsigned short);

      for (size_t i = 0; i < indices_accessor.count; ++i) {
        size_t idx =
            indices_accessor.byteOffset + indices_buffer_view.byteOffset + (i * indices_stride);

        if (!is_idx_in_range(idx, indices_buffer.data.size())) {
          _indices.resize(indices_start);
          continue;
        }

        _indices[indices_start + i] =
            *reinterpret_cast<const unsigned short*>(&(indices_buffer.data[idx]));
      }
    }
  } else {
    FEWI::error() << "Failed to get mesh #" << _id << " !";
    success = false;
  }

  if (!success) {
    FEWI::error() << "Failed to load mesh #" << _id << " from " << res.path() << " !";
  }

  updated();
}

void GltfMeshResource::resourcesUpdated() {}

void GltfMeshResource::ensureUpdated(Resources& resources) {
  checkAndApplyResourceUpdates(resources);
}

std::span<const Vertex> GltfMeshResource::getVertices() const {
  return _vertices;
}

std::span<const unsigned int> GltfMeshResource::getIndices() const {
  return _indices;
}

Resource& GltfMeshResource::asResource() {
  return *this;
}

Resource::Type GltfMeshResource::type() const {
  return Type::GltfMesh;
}