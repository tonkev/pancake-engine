#include "resources/gltf_primitive_resource.hpp"

#include "resources/gltf_resource.hpp"
#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/fewi.hpp"
#include "util/jsonify_component.hpp"

#include "tinygltf/tiny_gltf.h"

using namespace pancake;

Resources::Creator<GltfPrimitiveResource> gltf_mesh_resource_creator;

const Resource::Type GltfPrimitiveResource::TYPE = Type::GltfMesh;

GltfPrimitiveResource::GltfPrimitiveResource(std::string_view path, const GUID& guid)
    : JSONResource(path, guid) {}

void GltfPrimitiveResource::_load() {
  JSONResource::_load();
  if (const JSONValue* val = _json.get("gltf"); nullptr != val) {
    GUID gltf;
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&gltf, *val));
    setResourceGuid<GltfResource, GltfTag>(gltf);
  }
  if (const JSONValue* val = _json.get("mesh"); nullptr != val) {
    if (const JSONNumber* num = val->asNumber(); nullptr != num) {
      _mesh = static_cast<int>(num->getInt64());
    }
  }
  if (const JSONValue* val = _json.get("primitive"); nullptr != val) {
    if (const JSONNumber* num = val->asNumber(); nullptr != num) {
      _primitive = static_cast<int>(num->getInt64());
    }
  }
}

void GltfPrimitiveResource::_save() {
  _json.clear();

  TypeDescLibrary::get<GUID>().visit(JSONifyComponent(_json, "gltf", &getGltf()));
  _json.getOrCreate<JSONNumber>("mesh").set(static_cast<int64_t>(_mesh));
  _json.getOrCreate<JSONNumber>("primitive").set(static_cast<int64_t>(_primitive));

  JSONResource::_save();
}

void GltfPrimitiveResource::setGltf(const GUID& gltf) {
  setResourceGuid<GltfResource, GltfTag>(gltf);
  updated();
}

void GltfPrimitiveResource::setMesh(int mesh) {
  _mesh = mesh;
  updated();
}

void GltfPrimitiveResource::setPrimitive(int primitive) {
  _primitive = primitive;
  updated();
}

const pancake::GUID& GltfPrimitiveResource::getGltf() const {
  return getResourceGuid<GltfResource, GltfTag>();
}

int GltfPrimitiveResource::getMesh() const {
  return _mesh;
}

int GltfPrimitiveResource::getPrimitive() const {
  return _primitive;
}

template <typename T, T Vertex::*member>
bool fillVertexMember(std::vector<Vertex>& vertices,
                      const std::string& attrib_name,
                      const tinygltf::Primitive& primitive,
                      const tinygltf::Model& model) {
  const auto is_idx_in_range = [](int idx, int size) {
    if ((idx < 0) || (size <= idx)) {
      FEWI::error() << "Index out of range!";
      return false;
    }
    return true;
  };

  int attrib = -1;
  if (const auto it = primitive.attributes.find(attrib_name); it != primitive.attributes.end()) {
    attrib = it->second;
  }

  if (!is_idx_in_range(attrib, model.accessors.size())) {
    return false;
  }

  const auto& accessor = model.accessors[attrib];

  if (!is_idx_in_range(accessor.bufferView, model.bufferViews.size())) {
    return false;
  }

  if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
    FEWI::error() << "Vertex accessor component type is not float!";
    return false;
  }

  size_t stride;
  int components = 0;
  switch (accessor.type) {
    case TINYGLTF_TYPE_VEC2:
      stride = sizeof(Vec2f);
      components = 2;
      break;
    case TINYGLTF_TYPE_VEC3:
      stride = sizeof(Vec3f);
      components = 3;
      break;
    case TINYGLTF_TYPE_VEC4:
      stride = sizeof(Vec4f);
      components = 4;
      break;
    default:
      FEWI::error() << "Unsupported vertex accessor type (" << accessor.type << ")!";
      return false;
  }

  const auto& buffer_view = model.bufferViews[accessor.bufferView];

  if (!is_idx_in_range(buffer_view.buffer, model.buffers.size())) {
    return false;
  }

  const auto& buffer = model.buffers[buffer_view.buffer];

  if (vertices.size() < accessor.count) {
    vertices.resize(accessor.count);
  }

  if (0 < buffer_view.byteStride) {
    stride = buffer_view.byteStride;
  }

  for (size_t i = 0; i < accessor.count; ++i) {
    Vertex& vertex = vertices[i];
    size_t idx = accessor.byteOffset + buffer_view.byteOffset + (i * stride);

    if (!is_idx_in_range(idx, buffer.data.size())) {
      return false;
    }

    if constexpr (2 <= T::HEIGHT) {
      (vertex.*member).x() = *reinterpret_cast<const float*>(&(buffer.data[idx]));
      (vertex.*member).y() = *reinterpret_cast<const float*>(&(buffer.data[idx + sizeof(float)]));
    }

    if constexpr (3 <= T::HEIGHT) {
      if (3 <= components) {
        (vertex.*member).z() =
            *reinterpret_cast<const float*>(&(buffer.data[idx + (2 * sizeof(float))]));
      }
    }

    if constexpr (4 <= T::HEIGHT) {
      if (4 <= components) {
        (vertex.*member).w() =
            *reinterpret_cast<const float*>(&(buffer.data[idx + (3 * sizeof(float))]));
      }
    }
  }

  return true;
}

template <>
void GltfPrimitiveResource::resourceUpdated<GltfTag>(const GltfResource& res) {
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
  if ((0 <= _mesh) && (static_cast<size_t>(_mesh) < model.meshes.size())) {
    const auto& mesh = model.meshes[_mesh];
    if ((0 <= _primitive) && (static_cast<size_t>(_primitive) < mesh.primitives.size())) {
      const auto& primitive = mesh.primitives[_primitive];
      do {
        if (primitive.mode != 4) {
          FEWI::error() << "Unsupported primitive mode (" << primitive.mode << ")!";
          success = false;
          continue;
        }

        fillVertexMember<Vec4f, &Vertex::position>(_vertices, "POSITION", primitive, model);
        if (primitive.attributes.contains("NORMAL")) {
          fillVertexMember<Vec4f, &Vertex::normal>(_vertices, "NORMAL", primitive, model);
        }
        if (primitive.attributes.contains("TANGENT")) {
          fillVertexMember<Vec4f, &Vertex::tangent>(_vertices, "TANGENT", primitive, model);
        }
        if (primitive.attributes.contains("COLOR_0")) {
          fillVertexMember<Vec4f, &Vertex::color>(_vertices, "COLOR_0", primitive, model);
        }
        if (primitive.attributes.contains("TEXCOORD_0")) {
          fillVertexMember<Vec2f, &Vertex::uv0>(_vertices, "TEXCOORD_0", primitive, model);
        }
        if (primitive.attributes.contains("TEXCOORD_1")) {
          fillVertexMember<Vec2f, &Vertex::uv1>(_vertices, "TEXCOORD_1", primitive, model);
        }

        const auto& indices_accessor = model.accessors[primitive.indices];

        if (!is_idx_in_range(indices_accessor.bufferView, model.bufferViews.size())) {
          continue;
        }

        size_t indices_stride;
        switch (indices_accessor.componentType) {
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            indices_stride = sizeof(unsigned char);
            break;
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            indices_stride = sizeof(unsigned short);
            break;
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            indices_stride = sizeof(unsigned int);
            break;
          default:
            FEWI::error() << "Unsupported indices accessor type (" << indices_accessor.type << ")!";
            success = false;
            continue;
        }

        if (indices_accessor.type != TINYGLTF_TYPE_SCALAR) {
          FEWI::error() << "Indices accessor type is not scalar!";
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

        if (0 < indices_buffer_view.byteStride) {
          indices_stride = indices_buffer_view.byteStride;
        }

        for (size_t i = 0; i < indices_accessor.count; ++i) {
          unsigned int& index = _indices[indices_start + i];
          size_t idx =
              indices_accessor.byteOffset + indices_buffer_view.byteOffset + (i * indices_stride);

          if (!is_idx_in_range(idx, indices_buffer.data.size())) {
            _indices.resize(indices_start);
            continue;
          }

          switch (indices_accessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
              index = *reinterpret_cast<const unsigned char*>(&(indices_buffer.data[idx]));
              break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
              index = *reinterpret_cast<const unsigned short*>(&(indices_buffer.data[idx]));
              break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
              index = *reinterpret_cast<const unsigned int*>(&(indices_buffer.data[idx]));
              break;
            default:
              break;
          }
        }
      } while (false);
    } else {
      FEWI::error() << "Failed to get primitive #" << _primitive << " !";
      success = false;
    }
  } else {
    FEWI::error() << "Failed to get mesh #" << _mesh << " !";
    success = false;
  }

  if (!success) {
    FEWI::error() << "Failed to load primitive #" << _primitive << " from mesh #" << _mesh << " at "
                  << res.path() << " !";
  }

  updated();
}

void GltfPrimitiveResource::resourcesUpdated() {}

void GltfPrimitiveResource::ensureUpdated(Resources& resources) {
  checkAndApplyResourceUpdates(resources);
}

std::span<const Vertex> GltfPrimitiveResource::getVertices() const {
  return _vertices;
}

std::span<const unsigned int> GltfPrimitiveResource::getIndices() const {
  return _indices;
}

Resource& GltfPrimitiveResource::asResource() {
  return *this;
}

Resource::Type GltfPrimitiveResource::type() const {
  return Type::GltfMesh;
}