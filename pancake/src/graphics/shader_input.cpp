#include "graphics/shader_input.hpp"

#include "core/renderer.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "util/componentify_json.hpp"
#include "util/jsonify_component.hpp"

using namespace pancake;

ShaderInput::ShaderInput(std::string_view name, Value value) : _name(name), _value(value) {}

void ShaderInput::submitted(Renderer& renderer) const {
  if (std::holds_alternative<TextureRef>(_value)) {
    const auto& tex = std::get<TextureRef>(_value);
    if (tex.tile < 0) {
      if (!renderer.getTexture(tex.texture).has_value()) {
        renderer.queueTextureUpdate(tex.texture);
      }
    } else {
      const auto tileset_opt = renderer.getTileset(tex.texture);
      if (!tileset_opt.has_value()) {
        renderer.queueTilesetUpdate(tex.texture);
      }
    }
  }
}

void ShaderInput::bind(Shader& shader, Renderer& renderer) const {
  if (std::holds_alternative<int>(_value)) {
    shader.setUniform(_name, std::get<int>(_value));
  } else if (std::holds_alternative<float>(_value)) {
    shader.setUniform(_name, std::get<float>(_value));
  } else if (std::holds_alternative<Vec3f>(_value)) {
    shader.setUniform(_name, std::get<Vec3f>(_value));
  } else if (std::holds_alternative<Vec4f>(_value)) {
    shader.setUniform(_name, std::get<Vec4f>(_value));
  } else if (std::holds_alternative<Vec4u>(_value)) {
    shader.setUniform(_name, std::get<Vec4u>(_value));
  } else if (std::holds_alternative<Mat4f>(_value)) {
    shader.setUniform(_name, std::get<Mat4f>(_value));
  } else if (std::holds_alternative<TextureRef>(_value)) {
    const auto& tex = std::get<TextureRef>(_value);

    OptTextureConstRef texture_opt;
    OptTilesetConstRef tileset_opt;

    if (const auto framebuffer_opt = renderer.getFramebuffer(tex.texture);
        framebuffer_opt.has_value()) {
      texture_opt = framebuffer_opt.value().get().texture(tex.tile < 0 ? 0 : tex.tile);
    }

    if (!texture_opt.has_value()) {
      if (tex.tile < 0) {
        texture_opt = renderer.getTexture(tex.texture);
      } else {
        tileset_opt = renderer.getTileset(tex.texture);
        if (tileset_opt.has_value()) {
          texture_opt = renderer.getTexture(tileset_opt.value().get().getTextureProps());
        }
      }
    }

    const Texture& texture =
        texture_opt.has_value() ? texture_opt.value().get() : renderer.getBlankTexture();

    int slot = renderer.bindTexture(texture);
    shader.setUniform(_name, slot);
    shader.setUniform(_name + "_transform",
                      tileset_opt.has_value()
                          ? tileset_opt.value().get().genTransform(tex.tile, texture.transform())
                          : texture.transform());
  } else if (std::holds_alternative<LightInfo>(_value)) {
    const auto& light = std::get<LightInfo>(_value);

    shader.setUniform(_name + "_position", light.position);
    shader.setUniform(_name + "_color", light.color);
  }
}

ShaderInput::Type ShaderInput::getType() const {
  Type type = Type::Invalid;
  if (std::holds_alternative<int>(_value)) {
    type = Type::Int;
  } else if (std::holds_alternative<float>(_value)) {
    type = Type::Float;
  } else if (std::holds_alternative<Vec3f>(_value)) {
    type = Type::Vec3f;
  } else if (std::holds_alternative<Vec4f>(_value)) {
    type = Type::Vec4f;
  } else if (std::holds_alternative<Vec4u>(_value)) {
    type = Type::Vec4u;
  } else if (std::holds_alternative<Mat4f>(_value)) {
    type = Type::Mat4f;
  } else if (std::holds_alternative<TextureRef>(_value)) {
    type = Type::TextureRef;
  } else if (std::holds_alternative<LightInfo>(_value)) {
    type = Type::LightInfo;
  }
  return type;
}

std::string_view ShaderInput::getName() const {
  return _name;
}

void ShaderInput::toJson(JSONObject& json) const {
  Type type = getType();
  TypeDescLibrary::get<Type>().visit(JSONifyComponent(json, "type", &type));

  std::string& name = json.getOrCreate<JSONString>("name");
  name = _name;

  switch (type) {
    case ShaderInput::Type::Int:
      TypeDescLibrary::get<int>().visit(JSONifyComponent(json, "value", &std::get<int>(_value)));
      break;
    case ShaderInput::Type::Float:
      TypeDescLibrary::get<float>().visit(
          JSONifyComponent(json, "value", &std::get<float>(_value)));
      break;
    case ShaderInput::Type::Vec3f:
      TypeDescLibrary::get<Vec3f>().visit(
          JSONifyComponent(json, "value", &std::get<Vec3f>(_value)));
      break;
    case ShaderInput::Type::Vec4f:
      TypeDescLibrary::get<Vec4f>().visit(
          JSONifyComponent(json, "value", &std::get<Vec4f>(_value)));
      break;
    case ShaderInput::Type::Vec4u:
      TypeDescLibrary::get<Vec4u>().visit(
          JSONifyComponent(json, "value", &std::get<Vec4u>(_value)));
      break;
    case ShaderInput::Type::Mat4f:
      TypeDescLibrary::get<Mat4f>().visit(
          JSONifyComponent(json, "value", &std::get<Mat4f>(_value)));
      break;
    case ShaderInput::Type::TextureRef:
      TypeDescLibrary::get<TextureRef>().visit(
          JSONifyComponent(json, "value", &std::get<TextureRef>(_value)));
      break;
    case ShaderInput::Type::LightInfo:
      TypeDescLibrary::get<TextureRef>().visit(
          JSONifyComponent(json, "value", &std::get<LightInfo>(_value)));
      break;
    default:
      break;
  }
}

std::optional<ShaderInput> ShaderInput::fromJson(const JSONObject& json) {
  std::optional<ShaderInput> rtn = std::nullopt;

  if (const JSONValue* type_val = json.get("type"); nullptr != type_val) {
    Type type = Type::Invalid;
    TypeDescLibrary::get<Type>().visit(ComponentifyJSON(&type, *type_val));
    if (const JSONString* name = json.get<JSONString>("name"); nullptr != name) {
      if (const JSONValue* val = json.get("value"); nullptr != val) {
        int input_i;
        float input_f;
        Vec4f input_v;
        TextureRef input_t;
        switch (type) {
          case ShaderInput::Type::Int:
            TypeDescLibrary::get<int>().visit(ComponentifyJSON(&input_i, *val));
            rtn = ShaderInput(*name, input_i);
            break;
          case ShaderInput::Type::Float:
            TypeDescLibrary::get<float>().visit(ComponentifyJSON(&input_f, *val));
            rtn = ShaderInput(*name, input_f);
            break;
          case ShaderInput::Type::Vec3f:
            TypeDescLibrary::get<Vec3f>().visit(ComponentifyJSON(&input_v, *val));
            rtn = ShaderInput(*name, input_v);
            break;
          case ShaderInput::Type::Vec4f:
            TypeDescLibrary::get<Vec4f>().visit(ComponentifyJSON(&input_v, *val));
            rtn = ShaderInput(*name, input_v);
            break;
          case ShaderInput::Type::Vec4u:
            TypeDescLibrary::get<Vec4u>().visit(ComponentifyJSON(&input_v, *val));
            rtn = ShaderInput(*name, input_v);
            break;
          case ShaderInput::Type::Mat4f:
            TypeDescLibrary::get<Mat4f>().visit(ComponentifyJSON(&input_v, *val));
            rtn = ShaderInput(*name, input_v);
            break;
          case ShaderInput::Type::TextureRef:
            TypeDescLibrary::get<TextureRef>().visit(ComponentifyJSON(&input_t, *val));
            rtn = ShaderInput(*name, input_t);
            break;
          case ShaderInput::Type::LightInfo:
            TypeDescLibrary::get<LightInfo>().visit(ComponentifyJSON(&input_t, *val));
            rtn = ShaderInput(*name, input_t);
            break;
          default:
            break;
        }
      }
    }
  }

  return rtn;
}

bool ShaderInput::operator<(const ShaderInput& rhs) const {
  return std::tie(_name, _value) < std::tie(rhs._name, rhs._value);
}

bool ShaderInput::operator==(const ShaderInput& rhs) const {
  return std::tie(_name, _value) == std::tie(rhs._name, rhs._value);
}