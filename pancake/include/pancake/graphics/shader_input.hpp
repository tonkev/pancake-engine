#pragma once

#include "components/2d.hpp"
#include "graphics/light_info.hpp"
#include "pancake.hpp"
#include "util/matrix.hpp"

#include <optional>
#include <set>
#include <string>
#include <variant>

namespace pancake {
class Shader;
class Texture;
class Renderer;
class JSONObject;
class ShaderInput {
 public:
  using Value = std::variant<int, float, Vec3f, Vec4f, Vec4u, Mat4f, TextureRef, LightInfo>;

  enum class Type { Invalid, Int, Float, Vec3f, Vec4f, Vec4u, Mat4f, TextureRef, LightInfo };

  ShaderInput(std::string_view name, Value value);

  void submitted(Renderer& renderer) const;
  void bind(Shader& shader, Renderer& renderer) const;

  Type getType() const;
  std::string_view getName() const;

  void toJson(JSONObject& json) const;

  static std::optional<ShaderInput> fromJson(const JSONObject& json);

  bool operator<(const ShaderInput& rhs) const;
  bool operator==(const ShaderInput& rhs) const;

 private:
  std::string _name;
  Value _value;
};
}  // namespace pancake