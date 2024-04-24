#include "graphics/mesh.hpp"

using namespace pancake;

Mesh::Mesh(const GUID& guid) : _guid(guid) {
  setResourceGuid<MeshResourceInterface, MeshRes>(_guid);
}

template <>
void Mesh::resourceUpdated<MeshRes>(const MeshResourceInterface& res) {
  update(res.getVertices(), res.getIndices());
}

void Mesh::resourcesUpdated() {}

const GUID& Mesh::guid() const {
  return _guid;
}