#include "gl3/gl3_shader.hpp"

#include "resources/text_resource.hpp"
#include "util/fewi.hpp"

#include "GL/gl3w.h"

using namespace pancake;

static const size_t INFO_LOG_SIZE = 512;
static char info_log[INFO_LOG_SIZE];

GL3Shader::GL3Shader(const ShaderResourceInterface& res)
    : Shader(res.asResource().guid()), _program(0) {
  setResourceGuid<ShaderResourceInterface, ShaderSourceTag>(guid());
}

GL3Shader::~GL3Shader() {
  glDeleteShader(_vert_shader);
  glDeleteShader(_frag_shader);
  glDeleteProgram(_program);
}

void GL3Shader::compileVertexSource(std::string_view source) {
  glDeleteShader(_vert_shader);
  _vert_shader = glCreateShader(GL_VERTEX_SHADER);

  const char* vert_cstr = source.data();
  glShaderSource(_vert_shader, 1, &vert_cstr, NULL);
  glCompileShader(_vert_shader);

  int success;
  glGetShaderiv(_vert_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(_vert_shader, INFO_LOG_SIZE, NULL, info_log);
    FEWI::error() << "Failed to compile vertex shader! : " << info_log;
  }
}

void GL3Shader::compileFragmentSource(std::string_view source) {
  glDeleteShader(_frag_shader);
  _frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

  const char* frag_cstr = source.data();
  glShaderSource(_frag_shader, 1, &frag_cstr, NULL);
  glCompileShader(_frag_shader);

  int success;
  glGetShaderiv(_frag_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(_frag_shader, INFO_LOG_SIZE, NULL, info_log);
    FEWI::error() << "Failed to compile fragment shader! : " << info_log;
  }
}

void GL3Shader::linkProgram() {
  glDeleteProgram(_program);
  _program = glCreateProgram();

  glAttachShader(_program, _vert_shader);
  glAttachShader(_program, _frag_shader);
  glLinkProgram(_program);

  int success;
  glGetProgramiv(_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(_program, INFO_LOG_SIZE, NULL, info_log);
    FEWI::error() << "Failed to link shader program! : " << info_log;
  }
}

void GL3Shader::use() const {
  glUseProgram(_program);
}

void GL3Shader::setUniform(std::string_view name, int value) const {
  glUniform1i(glGetUniformLocation(_program, name.data()), value);
}

void GL3Shader::setUniform(std::string_view name, float value) const {
  glUniform1f(glGetUniformLocation(_program, name.data()), value);
}

void GL3Shader::setUniform(std::string_view name, const Vec3f& value) const {
  glUniform3f(glGetUniformLocation(_program, name.data()), value.x(), value.y(), value.z());
}

void GL3Shader::setUniform(std::string_view name, const Vec4f& value) const {
  glUniform4f(glGetUniformLocation(_program, name.data()), value.x(), value.y(), value.z(),
              value.w());
}

void GL3Shader::setUniform(std::string_view name, const Vec4u& value) const {
  glUniform4uiv(glGetUniformLocation(_program, name.data()), 1, &value.m[0][0]);
}

void GL3Shader::setUniform(std::string_view name, const Mat4f& value) const {
  glUniformMatrix4fv(glGetUniformLocation(_program, name.data()), 1, false, &(value.m[0][0]));
}