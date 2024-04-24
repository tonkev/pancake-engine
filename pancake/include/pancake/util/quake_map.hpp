#pragma once

#include "graphics/vertex.hpp"
#include "util/matrix.hpp"

#include <list>
#include <map>
#include <string>
#include <vector>

namespace pancake {
class QuakeMap {
 public:
  struct Token {
    enum class Type {
      Invalid,
      CurlyBracketOpen,
      CurlyBracketClosed,
      RoundBracketOpen,
      RoundBracketClosed,
      SquareBracketOpen,
      SquareBracketClosed,
      String,
      Number
    };

    std::string str{};
    Type type = Type::Invalid;

    static std::string_view getTypeName(Type type);
  };

  class Tokens : std::list<Token> {
   public:
    using Parent = std::list<Token>;

    using Parent::Parent;
    virtual ~Tokens() = default;

    void push(const Token& token);
    Token pop();

    Token::Type peek() const;

    using Parent::clear;
    using Parent::empty;
  };

  struct Face {
    Vec3f plane_a;
    Vec3f plane_b;
    Vec3f plane_c;
    std::string tex_name;
    Vec4f tex_u;
    Vec4f tex_v;
    float tex_rot;
    Vec2f tex_scale;

    bool parse(Tokens& tokens);
  };

  class Brush {
   public:
    Brush() = default;

    bool parse(Tokens& tokens);

   private:
    std::vector<Face> _faces;
  };

  class Entity {
   public:
    Entity() = default;

    bool parse(Tokens& tokens);

   private:
    std::map<std::string, std::string> _properties;
    std::vector<Brush> _brushes;
  };

  QuakeMap() = default;

  bool parse(Tokens& tokens);

  static bool tokenize(const std::string& text, Tokens& tokens);

  void genBrushesMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) const;

 private:
  std::vector<Entity> _entities;
};
}  // namespace pancake