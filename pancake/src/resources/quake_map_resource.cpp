#include "resources/quake_map_resource.hpp"

#include "resources/resources.hpp"

using namespace pancake;

Resources::Creator<QuakeMapResource> quake_map_resource_creator;

const Resource::Type QuakeMapResource::TYPE = Type::QuakeMap;

QuakeMapResource::QuakeMapResource(std::string_view path, const GUID& guid)
    : TextResource(path, guid) {}

void QuakeMapResource::_load() {
  TextResource::_load();

  QuakeMap::Tokens tokens;
  QuakeMap::tokenize(_text, tokens);
  _map.parse(tokens);
  _map.genBrushesMesh(_vertices, _indices);
}

std::span<const Vertex> QuakeMapResource::getVertices() const {
  return _vertices;
}

std::span<const unsigned int> QuakeMapResource::getIndices() const {
  return _indices;
}

Resource& QuakeMapResource::asResource() {
  return *this;
}

Resource::Type QuakeMapResource::type() const {
  return Type::QuakeMap;
}