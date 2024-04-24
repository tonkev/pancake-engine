#include "util/json.hpp"
#include "util/fewi.hpp"
#include "util/overloaded.hpp"

// #include <format>

using namespace pancake;

const JSONValue::Type JSONObject::TYPE = JSONValue::Type::Object;
const JSONValue::Type JSONArray::TYPE = JSONValue::Type::Array;
const JSONValue::Type JSONString::TYPE = JSONValue::Type::String;
const JSONValue::Type JSONNumber::TYPE = JSONValue::Type::Number;
const JSONValue::Type JSONBool::TYPE = JSONValue::Type::Bool;
const JSONValue::Type JSONNull::TYPE = JSONValue::Type::Null;

std::istream& operator>>(std::istream& is, JSONToken& token) {
  static const std::string specials = "{}[]\":, \n";

  char c;
  is >> c;

  token.clear();
  token += c;

  if (specials.find(c) != std::string::npos) {
    return is;
  }

  int p;
  while (!is.eof()) {
    p = is.peek();
    if (std::char_traits<int>::eof() != p) {
      if (specials.find(static_cast<char>(p)) != std::string::npos) {
        return is;
      }

      is >> std::noskipws >> c;
      token += c;
    }
  }

  return is;
}

JSONObject* JSONValue::asObject() {
  return nullptr;
}

const JSONObject* JSONValue::asObject() const {
  return nullptr;
}

JSONArray* JSONValue::asArray() {
  return nullptr;
}

const JSONArray* JSONValue::asArray() const {
  return nullptr;
}

JSONString* JSONValue::asString() {
  return nullptr;
}

const JSONString* JSONValue::asString() const {
  return nullptr;
}

JSONNumber* JSONValue::asNumber() {
  return nullptr;
}

const JSONNumber* JSONValue::asNumber() const {
  return nullptr;
}

JSONBool* JSONValue::asBool() {
  return nullptr;
}

const JSONBool* JSONValue::asBool() const {
  return nullptr;
}

const JSONNull* JSONValue::asNull() const {
  return nullptr;
}

JSONValue* JSONValue::parse(std::istream& is) {
  JSONValue* value = nullptr;

  is >> std::ws;

  bool ok = false;
  const int p = is.peek();
  if (std::char_traits<int>::eof() != p) {
    const char c = static_cast<char>(p);
    if (('-' == c) || std::isdigit(c)) {
      value = new JSONNumber(is, &ok);
    } else {
      switch (c) {
        case '{':
          value = new JSONObject(is, &ok);
          break;
        case '[':
          value = new JSONArray(is, &ok);
          break;
        case '"':
          value = new JSONString(is, &ok);
          break;
        case 'f':
          [[fallthrough]];
        case 't':
          value = new JSONBool(is, &ok);
          break;
        case 'n':
          value = new JSONNull(is, &ok);
          break;
        default:
          break;
      }
    }
  }

  if ((nullptr != value) && (!ok)) {
    delete value;
    value = nullptr;
  }

  return value;
}

JSONObject::JSONObject(std::istream& is, bool* success) {
  if (nullptr != success) {
    *success = false;
  }

  JSONToken token;
  is >> std::skipws >> token;

  if ("{" != token) {
    FEWI::error() << "Expected '{', found " << token << " at " << is.tellg() << " !";
    return;
  }

  bool first_iteration = true;
  while (!is.eof()) {
    is >> std::skipws >> token;
    if ("}" == token) {
      break;
    } else if ("," == token) {
      if (first_iteration) {
        FEWI::error() << "Unexpected ',' at " << is.tellg() << " !";
      }
    } else if ("\"" == token) {
      bool ok = false;
      JSONString key(is, &ok, token);

      if (!ok) {
        return;
      }

      is >> std::skipws >> token;
      if (":" != token) {
        FEWI::error() << "Expected ':', found " << token << " at " << is.tellg() << " !";
        return;
      }

      JSONValue* value = JSONValue::parse(is);
      if (nullptr == value) {
        return;
      }

      _map.emplace(key, value);
    }

    first_iteration = false;
  }

  if (("}" == token) && (nullptr != success)) {
    *success = true;
  }
}

const JSONValue* JSONObject::get(std::string_view key) const {
  if (const auto it = _map.find(key); (it != _map.end())) {
    return it->second.get();
  }
  return nullptr;
}

const JSONValue& JSONObject::operator[](std::string_view key) const {
  static const JSONString fallback;
  if (_map.contains(key)) {
    return *_map.find(key)->second.get();
  }
  return fallback;
}

void JSONObject::clear() {
  _map.clear();
}

const JSONObject::Map& JSONObject::pairs() const {
  return _map;
}

JSONObject* JSONObject::asObject() {
  return this;
}

const JSONObject* JSONObject::asObject() const {
  return this;
}

void JSONObject::serialise(std::ostream& os, std::string_view indent) const {
  os << '{' << std::endl;

  size_t c = 0;
  std::string indent_plus = std::string(indent) + "  ";
  for (const auto& [key, value] : _map) {
    os << indent_plus << '"' << key << "\" : ";
    value->serialise(os, indent_plus);
    if (_map.size() != ++c) {
      os << ",";
    }
    os << std::endl;
  }

  os << indent << '}';
}

JSONValue::Type JSONObject::getType() const {
  return TYPE;
}

JSONArray::JSONArray(std::istream& is, bool* success) {
  if (nullptr != success) {
    *success = false;
  }

  char c;
  is >> std::skipws >> c;

  if ('[' != c) {
    FEWI::error() << "Expected '[', found " << c << " at " << is.tellg() << " !";
    return;
  }

  int p;
  bool expecting_comma = false;
  while (!is.eof()) {
    is >> std::ws;
    p = is.peek();

    if (is.eof()) {
      break;
    }

    c = static_cast<char>(p);
    if (']' == c) {
      is >> c;
      break;
    } else if (',' == c) {
      is >> c;

      if (!expecting_comma) {
        FEWI::error() << "Unexpected ',' at " << is.tellg() << " !";
        break;
      }

      expecting_comma = false;
    } else {
      if (expecting_comma) {
        FEWI::error() << "Expected ',', found " << c << " at " << is.tellg() << " !";
        c = '0';
        break;
      }

      JSONValue* value = JSONValue::parse(is);
      if (nullptr == value) {
        return;
      }

      _values.emplace_back(value);

      expecting_comma = true;
    }
  }

  if ((']' == c) && (nullptr != success)) {
    *success = true;
  }
}

const JSONValue& JSONArray::operator[](size_t index) const {
  static const JSONNull fallback;
  if (index < _values.size()) {
    return *_values[index].get();
  }
  return fallback;
}

void JSONArray::clear() {
  _values.clear();
}

size_t JSONArray::size() const {
  return _values.size();
}

JSONArray* JSONArray::asArray() {
  return this;
}

const JSONArray* JSONArray::asArray() const {
  return this;
}

void JSONArray::serialise(std::ostream& os, std::string_view indent) const {
  os << '[' << std::endl;

  size_t c = 0;
  std::string indent_plus = std::string(indent) + "  ";
  for (const auto& value : _values) {
    os << indent_plus;
    value->serialise(os, indent_plus);
    if (_values.size() != ++c) {
      os << ",";
    }
    os << std::endl;
  }

  os << indent << ']';
}

JSONValue::Type JSONArray::getType() const {
  return TYPE;
}

JSONString::JSONString(std::istream& is, bool* success, const JSONToken& first_token) {
  if (nullptr != success) {
    *success = false;
  }

  JSONToken token = first_token;
  if (token.empty()) {
    is >> std::skipws >> token;
  }

  if ("\"" != token) {
    FEWI::error() << "Expected '\"', found " << token << " at " << is.tellg() << " !";
    return;
  }

  while (!is.eof()) {
    is >> std::noskipws >> token;
    if ("\"" == token) {
      break;
    }
    *this += token;
  }

  if (is.eof()) {
    FEWI::error() << "Failed to find closing '\"' at " << is.tellg() << " !";
    return;
  }

  if (("\"" == token) && (nullptr != success)) {
    *success = true;
  }
}

JSONString* JSONString::asString() {
  return this;
}

const JSONString* JSONString::asString() const {
  return this;
}

void JSONString::serialise(std::ostream& os, std::string_view indent) const {
  os << '"' << *this << '"';
}

JSONValue::Type JSONString::getType() const {
  return TYPE;
}

JSONNumber::JSONNumber(std::istream& is, bool* success) {
  if (nullptr != success) {
    *success = false;
  }

  std::string num_str;

  bool isFloating = false;
  bool isSigned = false;

  do {
    char c = static_cast<char>(is.peek());
    if ('-' == c) {
      isSigned = true;
      num_str += c;

      is >> std::noskipws >> c;
      c = static_cast<char>(is.peek());
    }

    if ((!std::isdigit(c)) || is.eof()) {
      return;
    }

    if ('0' != c) {
      while (std::isdigit(c)) {
        num_str += c;
        is >> std::noskipws >> c;
        c = static_cast<char>(is.peek());

        if (is.eof()) {
          break;
        }
      }
    } else {
      num_str += c;
      is >> std::noskipws >> c;
      c = static_cast<char>(is.peek());
    }

    if (is.eof()) {
      break;
    }

    if ('.' == c) {
      isFloating = true;

      num_str += c;
      is >> std::noskipws >> c;
      c = static_cast<char>(is.peek());
      if (!std::isdigit(c)) {
        return;
      }

      while (std::isdigit(c)) {
        num_str += c;
        is >> std::noskipws >> c;
        c = static_cast<char>(is.peek());
        if (is.eof()) {
          break;
        }
      }

      if (is.eof()) {
        break;
      }
    }

    if (('e' == c) || ('E' == c)) {
      num_str += c;
      is >> std::noskipws >> c;
      c = static_cast<char>(is.peek());

      if (('-' == c) || ('+' == c)) {
        isFloating = isFloating || ('-' == c);

        num_str += c;
        is >> std::noskipws >> c;
        c = static_cast<char>(is.peek());
      }

      if (!std::isdigit(c)) {
        return;
      }

      while (std::isdigit(c)) {
        num_str += c;
        is >> std::noskipws >> c;
        c = static_cast<char>(is.peek());
        if (is.eof()) {
          break;
        }
      }
    }
  } while (false);

  if (isFloating) {
    _num = std::stod(num_str);
  } else if (isSigned) {
    _num = std::stoll(num_str);
  } else {
    _num = std::stoull(num_str);
  }

  if (nullptr != success) {
    *success = true;
  }
}

void JSONNumber::set(int64_t num) {
  _num = num;
}

void JSONNumber::set(uint64_t num) {
  _num = num;
}

void JSONNumber::set(double num) {
  _num = num;
}

int64_t JSONNumber::getInt64() const {
  int64_t ret;
  std::visit(overloaded{[&ret](auto num) { ret = static_cast<int64_t>(num); },
                        [&ret](int64_t num) { ret = num; }},
             _num);
  return ret;
}

uint64_t JSONNumber::getUInt64() const {
  uint64_t ret;
  std::visit(overloaded{[&ret](auto num) { ret = static_cast<uint64_t>(num); },
                        [&ret](uint64_t num) { ret = num; }},
             _num);
  return ret;
}

double JSONNumber::getDouble() const {
  double ret;
  std::visit(overloaded{[&ret](auto num) { ret = static_cast<double>(num); },
                        [&ret](double num) { ret = num; }},
             _num);
  return ret;
}

JSONNumber* JSONNumber::asNumber() {
  return this;
}

const JSONNumber* JSONNumber::asNumber() const {
  return this;
}

void JSONNumber::serialise(std::ostream& os, std::string_view indent) const {
  std::visit([&os](auto num) { os << num; }, _num);
}

JSONValue::Type JSONNumber::getType() const {
  return TYPE;
}

JSONBool::JSONBool(std::istream& is, bool* success) {
  JSONToken token;
  is >> std::skipws >> token;

  _value = ("true" == token);

  if (nullptr != success) {
    *success = (("true" == token) || ("false" == token));
  }
}

void JSONBool::set(bool value) {
  _value = value;
}

bool JSONBool::get() const {
  return _value;
}

JSONBool* JSONBool::asBool() {
  return this;
}

const JSONBool* JSONBool::asBool() const {
  return this;
}

void JSONBool::serialise(std::ostream& os, std::string_view indent) const {
  os << _value ? "true" : "false";
}

JSONValue::Type JSONBool::getType() const {
  return TYPE;
}

JSONNull::JSONNull(std::istream& is, bool* success) {
  JSONToken token;
  is >> std::skipws >> token;

  if (nullptr != success) {
    *success = ("null" == token);
  }

  if (("null" != token)) {
    *success = false;
  }
}

const JSONNull* JSONNull::asNull() const {
  return this;
}

void JSONNull::serialise(std::ostream& os, std::string_view indent) const {
  os << "null";
}

JSONValue::Type JSONNull::getType() const {
  return TYPE;
}