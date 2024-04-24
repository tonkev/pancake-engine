#pragma once

#include <string>

#include "pancake.hpp"
#include "resources/resource_user.hpp"
#include "resources/shader_resource_interface.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"

namespace pancake {
struct ShaderSourceTag {};

class Shader
    : public std::enable_shared_from_this<Shader>,
      public ResourceUser<Shader, ResourceUse<Shader, ShaderResourceInterface, ShaderSourceTag>> {
 public:
  virtual ~Shader() = default;

  virtual void compileVertexSource(std::string_view source) = 0;
  virtual void compileFragmentSource(std::string_view source) = 0;
  virtual void linkProgram() = 0;

  virtual void use() const = 0;

  virtual void setUniform(std::string_view name, int value) const = 0;
  virtual void setUniform(std::string_view name, float value) const = 0;
  virtual void setUniform(std::string_view name, const Vec3f& value) const = 0;
  virtual void setUniform(std::string_view name, const Vec4f& value) const = 0;
  virtual void setUniform(std::string_view name, const Vec4u& value) const = 0;
  virtual void setUniform(std::string_view name, const Mat4f& value) const = 0;

  template <typename T>
  void resourceUpdated(const ShaderResourceInterface& res);

  void resourcesUpdated();

  const GUID& guid() const;

  Ptr<Shader> ptr();
  Ptr<const Shader> ptr() const;

  WeakPtr<Shader> weakPtr();
  WeakPtr<const Shader> weakPtr() const;

 protected:
  Shader(const GUID& guid);

 private:
  GUID _guid;
};
}  // namespace pancake