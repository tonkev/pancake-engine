#include "graphics/shader.hpp"

using namespace pancake;

Shader::Shader(const GUID& guid) : _guid(guid) {}

template <>
void Shader::resourceUpdated<ShaderSourceTag>(const ShaderResourceInterface& res) {
  compileVertexSource(res.getVertexSource());
  compileFragmentSource(res.getFragmentSource());
}

void Shader::resourcesUpdated() {
  linkProgram();
}

const GUID& Shader::guid() const {
  return _guid;
}

Ptr<Shader> Shader::ptr() {
  return shared_from_this();
}
Ptr<const Shader> Shader::ptr() const {
  return shared_from_this();
}

WeakPtr<Shader> Shader::weakPtr() {
  return weak_from_this();
}
WeakPtr<const Shader> Shader::weakPtr() const {
  return weak_from_this();
}