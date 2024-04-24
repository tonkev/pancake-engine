#include "util/quake_map.hpp"

#include "util/fewi.hpp"

#include <cctype>
#include <initializer_list>
#include <map>

using namespace pancake;

std::string_view QuakeMap::Token::getTypeName(Type type) {
  switch (type) {
    default:
    case Type::Invalid:
      return "Invalid";
    case Type::CurlyBracketOpen:
      return "CurlyBracketOpen";
    case Type::CurlyBracketClosed:
      return "CurlyBracketClosed";
    case Type::RoundBracketOpen:
      return "RoundBracketOpen";
    case Type::RoundBracketClosed:
      return "RoundBracketClosed";
    case Type::SquareBracketOpen:
      return "SquareBracketOpen";
    case Type::SquareBracketClosed:
      return "SquareBracketClosed";
    case Type::String:
      return "String";
    case Type::Number:
      return "Number";
  }
}

void QuakeMap::Tokens::push(const Token& token) {
  emplace_back(token);
}

QuakeMap::Token QuakeMap::Tokens::pop() {
  Token token;
  if (!empty()) {
    token = front();
    pop_front();
  }
  return token;
}

QuakeMap::Token::Type QuakeMap::Tokens::peek() const {
  Token::Type type = Token::Type::Invalid;
  if (!empty()) {
    type = front().type;
  }
  return type;
}

struct ExpectFoundMessage {
  QuakeMap::Token::Type expected;
  QuakeMap::Token::Type found;
};

std::ostream& operator<<(std::ostream& os, const ExpectFoundMessage& ef) {
  os << "Expected " << QuakeMap::Token::getTypeName(ef.expected) << ", found "
     << QuakeMap::Token::getTypeName(ef.found) << "!";
  return os;
}

bool QuakeMap::Face::parse(Tokens& tokens) {
  static const char generic_fail_msg[] = "Failed to parse Face: ";

  const auto parse_float = [&](float& f) -> bool {
    Token token = tokens.pop();
    if (Token::Type::Number != token.type) {
      FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::Number, token.type);
      return false;
    }

    f = std::stof(token.str);
    return true;
  };

  const auto parse_plane_coord = [&](Vec3f& coord) -> bool {
    if (Token::Type type = tokens.pop().type; Token::Type::RoundBracketOpen != type) {
      FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::RoundBracketOpen, type);
      return false;
    }

    for (float& f : coord.m[0]) {
      if (!parse_float(f)) {
        return false;
      }
    }

    if (Token::Type type = tokens.pop().type; Token::Type::RoundBracketClosed != type) {
      FEWI::error() << generic_fail_msg
                    << ExpectFoundMessage(Token::Type::RoundBracketClosed, type);
      return false;
    }

    return true;
  };

  const auto parse_tex_offset = [&](Vec4f& offset) -> bool {
    if (Token::Type type = tokens.pop().type; Token::Type::SquareBracketOpen != type) {
      FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::SquareBracketOpen, type);
      return false;
    }

    for (float& f : offset.m[0]) {
      if (!parse_float(f)) {
        return false;
      }
    }

    if (Token::Type type = tokens.pop().type; Token::Type::SquareBracketClosed != type) {
      FEWI::error() << generic_fail_msg
                    << ExpectFoundMessage(Token::Type::SquareBracketClosed, type);
      return false;
    }

    return true;
  };

  if ((!parse_plane_coord(plane_a)) || (!parse_plane_coord(plane_b)) ||
      (!parse_plane_coord(plane_c))) {
    return false;
  }

  if (Token token = tokens.pop(); Token::Type::String == token.type) {
    tex_name = token.str;
  } else {
    FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::String, token.type);
    return false;
  }

  if ((!parse_tex_offset(tex_u)) || (!parse_tex_offset(tex_v))) {
    return false;
  }

  if ((!parse_float(tex_rot)) || (!parse_float(tex_scale.x())) || (!parse_float(tex_scale.y()))) {
    return false;
  }

  return true;
}

bool QuakeMap::Brush::parse(Tokens& tokens) {
  static const char generic_fail_msg[] = "Failed to parse Brush: ";

  _faces.clear();

  if (Token::Type type = tokens.pop().type; Token::Type::CurlyBracketOpen != type) {
    FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::CurlyBracketOpen, type);
    return false;
  }

  while (Token::Type::RoundBracketOpen == tokens.peek()) {
    Face& face = _faces.emplace_back();
    if (!face.parse(tokens)) {
      return false;
    }
  }

  if (Token::Type type = tokens.pop().type; Token::Type::CurlyBracketClosed != type) {
    FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::CurlyBracketClosed, type);
    return false;
  }

  return true;
}

bool QuakeMap::Entity::parse(Tokens& tokens) {
  static const char generic_fail_msg[] = "Failed to parse Entity: ";

  _properties.clear();
  _brushes.clear();

  if (Token::Type type = tokens.pop().type; Token::Type::CurlyBracketOpen != type) {
    FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::CurlyBracketOpen, type);
    return false;
  }

  while (Token::Type::String == tokens.peek()) {
    Token key = tokens.pop();
    Token value = tokens.pop();
    if (Token::Type::String != value.type) {
      FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::String, value.type);
      return false;
    }

    _properties.emplace(key.str, value.str);
  }

  while (Token::Type::CurlyBracketOpen == tokens.peek()) {
    Brush& brush = _brushes.emplace_back();
    if (!brush.parse(tokens)) {
      return false;
    }
  }

  if (Token::Type type = tokens.pop().type; Token::Type::CurlyBracketClosed != type) {
    FEWI::error() << generic_fail_msg << ExpectFoundMessage(Token::Type::CurlyBracketClosed, type);
    return false;
  }

  return true;
}

bool QuakeMap::parse(Tokens& tokens) {
  _entities.clear();

  do {
    Entity& entity = _entities.emplace_back();
    if (!entity.parse(tokens)) {
      return false;
    }
  } while (!tokens.empty());

  return true;
}

bool QuakeMap::tokenize(const std::string& text, Tokens& tokens) {
  static const char generic_fail_msg[] = "Failed to tokenize: ";

  tokens.clear();

  Token token;
  auto push_token = [&tokens, &token]() {
    if ((Token::Type::Invalid != token.type) || (!token.str.empty())) {
      tokens.push(token);
    }
    token = Token();
  };

  bool in_comment = false;
  bool in_quotes = false;
  char prev = '\0';
  for (size_t i = 0; i < text.size(); ++i) {
    char c = text[i];
    char next = ((i + 1) != text.size()) ? text[i + 1] : '\0';

    if (in_comment) {
      if ('\n' == c) {
        in_comment = false;
      }
    } else if (in_quotes) {
      if ('"' == c) {
        in_quotes = false;
        push_token();
      } else if (std::isprint(c)) {
        token.str.push_back(c);
      } else {
        FEWI::error() << generic_fail_msg << "Unexpected character in quotes : " << c;
        return false;
      }
    } else if (('/' == c) && ('/' == next)) {
      push_token();
      in_comment = true;
    } else if (std::isspace(c)) {
      if (!std::isspace(prev)) {
        push_token();
      }
    } else if ('{' == c) {
      push_token();
      token.type = Token::Type::CurlyBracketOpen;
      token.str = c;
      push_token();
    } else if ('}' == c) {
      push_token();
      token.type = Token::Type::CurlyBracketClosed;
      token.str = c;
      push_token();
    } else if ('(' == c) {
      push_token();
      token.type = Token::Type::RoundBracketOpen;
      token.str = c;
      push_token();
    } else if (')' == c) {
      push_token();
      token.type = Token::Type::RoundBracketClosed;
      token.str = c;
      push_token();
    } else if ('[' == c) {
      push_token();
      token.type = Token::Type::SquareBracketOpen;
      token.str = c;
      push_token();
    } else if (']' == c) {
      push_token();
      token.type = Token::Type::SquareBracketClosed;
      token.str = c;
      push_token();
    } else if ('"' == c) {
      if (token.str.empty()) {
        token.type = Token::Type::String;
        in_quotes = true;
      } else {
        FEWI::error() << generic_fail_msg << "Unexpected \"!";
        return false;
      }
    } else if ('-' == c) {
      token.type = token.str.empty() ? Token::Type::Number : Token::Type::String;
      token.str.push_back(c);
    } else if ('.' == c) {
      if (token.str.empty()) {
        token.type = Token::Type::String;
      }
      token.str.push_back(c);
    } else if (std::isdigit(c)) {
      if (token.str.empty()) {
        token.type = Token::Type::Number;
      }
      token.str.push_back(c);
    } else if (std::isalpha(c) || std::ispunct(c)) {
      token.type = Token::Type::String;
      token.str.push_back(c);
    } else {
      FEWI::error() << generic_fail_msg << "Unexpected character: " << c;
      return false;
    }

    prev = c;
  }

  return true;
}

class BrushFace;

class BrushEdge {
 public:
  BrushEdge(const Vec3f& a, const Vec3f& b) : _a(a), _b(b) {}

  void sliceByPlane(const Vec3f& point, const Vec3f& normal);
  void addFace(BrushFace& face);

  bool connecting(const BrushEdge& other) const {
    return _a.approximatelyEqual(other._a, 0.001f) || _a.approximatelyEqual(other._b, 0.001f) ||
           _b.approximatelyEqual(other._a, 0.001f) || _b.approximatelyEqual(other._b, 0.001f);
  }

  // assumes other connects to this
  // j should always be the connecting Vertex between edges
  void initTriangle(const BrushEdge& other, Vertex& i, Vertex& j, Vertex& k) const {
    if (_a.approximatelyEqual(other._a, 0.001f)) {
      i.position = Vec4f(_b, 1.f);
      j.position = Vec4f(_a, 1.f);
      k.position = Vec4f(other._b, 1.f);
    } else if (_a.approximatelyEqual(other._b, 0.001f)) {
      i.position = Vec4f(_b, 1.f);
      j.position = Vec4f(_a, 1.f);
      k.position = Vec4f(other._a, 1.f);
    } else if (_b.approximatelyEqual(other._a, 0.001f)) {
      i.position = Vec4f(_a, 1.f);
      j.position = Vec4f(_b, 1.f);
      k.position = Vec4f(other._b, 1.f);
    } else {
      i.position = Vec4f(_a, 1.f);
      j.position = Vec4f(_b, 1.f);
      k.position = Vec4f(other._a, 1.f);
    }
  }

  Vec3f& a() { return _a; }
  const Vec3f& a() const { return _a; }

  Vec3f& b() { return _b; }
  const Vec3f& b() const { return _b; }

 private:
  Vec3f _a;
  Vec3f _b;
  std::vector<std::reference_wrapper<BrushFace>> _faces;
};

class BrushFace {
 public:
  // assumes edges are sequential !
  BrushFace(std::initializer_list<std::reference_wrapper<BrushEdge>> edges) {
    for (BrushEdge& edge : edges) {
      _edges.emplace_back(edge);
      edge.addFace(*this);
    }
  };

  void triangulate(std::vector<Vertex>& vertices,
                   std::vector<unsigned int>& indices,
                   std::map<Vertex, unsigned int>& vertex_to_index) const {
    static const auto getOrCreateIndex = [](const Vertex& vertex, std::vector<Vertex>& vertices,
                                            std::map<Vertex, unsigned int>& vertex_to_index) {
      if (const auto it = vertex_to_index.find(vertex); it != vertex_to_index.end()) {
        return it->second;
      } else {
        unsigned int index = vertices.size();
        vertex_to_index.emplace(vertex, vertices.size());
        vertices.push_back(vertex);
        return index;
      }
    };

    if (1 < _edges.size()) {
      Vertex i, j, k;
      BrushEdge link(_edges[0].get().a(), _edges[0].get().b());
      const BrushEdge* next;
      int a = 1;
      int b = _edges.size() - 1;
      int c = 0;
      while (a < b) {
        if (1 == (b - a)) {
          const BrushEdge& edge_a = _edges[a++];
          link.a() = edge_a.a();
          link.b() = edge_a.b();
          next = &(_edges[b--].get());
        } else {
          next = &(_edges[(((++c) % 2) == 0) ? a++ : b--].get());
        }
        next->initTriangle(link, i, j, k);
        indices.push_back(getOrCreateIndex(i, vertices, vertex_to_index));
        indices.push_back(getOrCreateIndex(j, vertices, vertex_to_index));
        indices.push_back(getOrCreateIndex(k, vertices, vertex_to_index));
        link.a() = i.position.xyz();
        link.b() = k.position.xyz();
      }
    }
  }

 private:
  // assumes new edge always connects to another
  void addEdge(BrushEdge& edge) {
    if (_edges.empty()) {
      _edges.emplace_back(edge);
      edge.addFace(*this);
    } else if (1 == _edges.size()) {
      if (edge.connecting(_edges[0])) {
        _edges.emplace_back(edge);
        edge.addFace(*this);
      }
    } else {
      for (size_t i = 0; i < _edges.size(); ++i) {
        size_t j = (i + 1) % _edges.size();
        if (edge.connecting(_edges[i]) && edge.connecting(_edges[j])) {
          _edges.emplace(_edges.begin() + j, edge);
          edge.addFace(*this);
        }
      }
    }
  }

  std::vector<std::reference_wrapper<BrushEdge>> _edges;
};

void BrushEdge::sliceByPlane(const Vec3f& point, const Vec3f& normal) {}

void BrushEdge::addFace(BrushFace& face) {
  _faces.emplace_back(face);
}

void QuakeMap::genBrushesMesh(std::vector<Vertex>& vertices,
                              std::vector<unsigned int>& indices) const {
  vertices.clear();
  indices.clear();

  // initialise to BIG cube
  const float big = 1.f;
  std::vector<BrushEdge> edges = {BrushEdge(Vec3f(-big, -big, -big), Vec3f(-big, -big, big)),
                                  BrushEdge(Vec3f(-big, -big, big), Vec3f(-big, big, big)),
                                  BrushEdge(Vec3f(-big, big, big), Vec3f(-big, big, -big)),
                                  BrushEdge(Vec3f(-big, big, -big), Vec3f(-big, -big, -big)),
                                  BrushEdge(Vec3f(big, -big, -big), Vec3f(big, -big, big)),
                                  BrushEdge(Vec3f(big, -big, big), Vec3f(big, big, big)),
                                  BrushEdge(Vec3f(big, big, big), Vec3f(big, big, -big)),
                                  BrushEdge(Vec3f(big, big, -big), Vec3f(big, -big, -big)),
                                  BrushEdge(Vec3f(-big, -big, -big), Vec3f(big, -big, -big)),
                                  BrushEdge(Vec3f(-big, -big, big), Vec3f(big, -big, big)),
                                  BrushEdge(Vec3f(-big, big, big), Vec3f(big, big, big)),
                                  BrushEdge(Vec3f(-big, big, -big), Vec3f(big, big, -big))};

  std::vector<BrushFace> faces = {BrushFace({edges[0], edges[1], edges[2], edges[3]}),
                                  BrushFace({edges[4], edges[5], edges[6], edges[7]}),
                                  BrushFace({edges[0], edges[9], edges[4], edges[8]}),
                                  BrushFace({edges[1], edges[10], edges[5], edges[9]}),
                                  BrushFace({edges[2], edges[11], edges[6], edges[10]}),
                                  BrushFace({edges[3], edges[11], edges[7], edges[8]})};

  std::map<Vertex, unsigned int> vertex_to_index;
  for (const BrushFace& face : faces) {
    face.triangulate(vertices, indices, vertex_to_index);
  }
}