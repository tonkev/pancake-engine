#include "resources/gltf_resource.hpp"

#include "resources/resources.hpp"
#include "util/fewi.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#include "tinygltf/tiny_gltf.h"

using namespace pancake;

Resources::Creator<GltfResource> gltf_resource_creator;

const Resource::Type GltfResource::TYPE = Type::Gltf;

bool loadImage(tinygltf::Image*,
               const int,
               std::string*,
               std::string*,
               int,
               int,
               const unsigned char*,
               int,
               void* user_pointer) {
  // do nothing
  return true;
}

GltfResource::GltfResource(std::string_view path, const GUID& guid)
    : Resource(path, guid), _model(new tinygltf::Model()) {}

void GltfResource::_load() {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  loader.SetImageLoader(loadImage, nullptr);
  bool success = loader.LoadASCIIFromFile(_model.get(), &err, &warn, _full_path);

  if (!warn.empty()) {
    FEWI::warn() << warn;
  }

  if (!err.empty()) {
    FEWI::error() << err;
  }

  if (!success) {
    FEWI::error() << "Failed to parse glTF : " << _full_path;
  }
}

void GltfResource::_save() {}

const tinygltf::Model& GltfResource::getModel() const {
  return *_model;
}

Resource::Type GltfResource::type() const {
  return Type::Gltf;
}