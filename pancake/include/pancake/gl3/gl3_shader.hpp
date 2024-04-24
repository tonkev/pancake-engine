#pragma once

#include "graphics/shader.hpp"

#include "pancake.hpp"
#include "util/guid.hpp"

#include <string>

namespace pancake {
class GL3Renderer;
class TextResource;
class GL3Shader : public Shader {
 public:
  virtual ~GL3Shader();

  virtual void compileVertexSource(std::string_view source) override;
  virtual void compileFragmentSource(std::string_view source) override;
  virtual void linkProgram() override;

  virtual void use() const override;

  virtual void setUniform(std::string_view name, int value) const override;
  virtual void setUniform(std::string_view name, float value) const override;
  virtual void setUniform(std::string_view name, const Vec3f& value) const override;
  virtual void setUniform(std::string_view name, const Vec4f& value) const override;
  virtual void setUniform(std::string_view name, const Vec4u& value) const override;
  virtual void setUniform(std::string_view name, const Mat4f& value) const override;

 private:
  GL3Shader(const ShaderResourceInterface& res);

  friend GL3Renderer;

  unsigned int _vert_shader;
  unsigned int _frag_shader;
  unsigned int _program;
};
}  // namespace pancake