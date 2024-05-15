#pragma once

#include <concepts>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace pancake {
class JSONObject;
class JSONArray;
class JSONString;
class JSONNumber;
class JSONBool;
class JSONNull;

class JSONToken : public std::string {};

class JSONValue {
 public:
  enum class Type { Object, Array, String, Number, Bool, Null };

  virtual ~JSONValue() = default;

  virtual JSONObject* asObject();
  virtual const JSONObject* asObject() const;

  virtual JSONArray* asArray();
  virtual const JSONArray* asArray() const;

  virtual JSONString* asString();
  virtual const JSONString* asString() const;

  virtual JSONNumber* asNumber();
  virtual const JSONNumber* asNumber() const;

  virtual JSONBool* asBool();
  virtual const JSONBool* asBool() const;

  virtual const JSONNull* asNull() const;

  virtual void serialise(std::ostream& os, std::string_view indent = "") const = 0;
  virtual Type getType() const = 0;

  static JSONValue* parse(std::istream& is);

 protected:
  JSONValue() = default;
};

template <typename T>
concept JSONValuable = std::is_base_of_v<JSONValue, T>;

class JSONObject : public JSONValue {
 public:
  using Map = std::map<std::string, std::unique_ptr<JSONValue>, std::less<>>;

  JSONObject() = default;
  JSONObject(std::istream& is, bool* success = nullptr);
  JSONObject(JSONObject&&) = default;
  virtual ~JSONObject() override = default;

  JSONObject& operator=(JSONObject&&) = default;

  template <JSONValuable T>
  T& getOrCreate(std::string_view key) {
    if (const auto it = _map.find(key); (it != _map.end())) {
      if (JSONValue& value = *it->second.get(); (T::TYPE == value.getType())) {
        return static_cast<T&>(value);
      }
      _map.erase(it);
    }

    return static_cast<T&>(*_map.emplace(key, new T()).first->second.get());
  };

  template <JSONValuable T>
  const T* get(std::string_view key) const {
    if (const JSONValue* value = get(key); (nullptr != value) && (T::TYPE == value->getType())) {
      return static_cast<const T*>(value);
    }
    return nullptr;
  }

  void clear();

  const JSONValue* get(std::string_view key) const;

  const JSONValue& operator[](std::string_view key) const;

  const Map& pairs() const;

  virtual JSONObject* asObject() override;
  virtual const JSONObject* asObject() const override;

  virtual void serialise(std::ostream& os, std::string_view indent = "") const override;
  virtual Type getType() const override;

  static const Type TYPE;

 private:
  std::map<std::string, std::unique_ptr<JSONValue>, std::less<>> _map;
};

class JSONArray : public JSONValue {
 public:
  JSONArray() = default;
  JSONArray(std::istream& is, bool* success = nullptr);
  virtual ~JSONArray() override = default;

  template <JSONValuable T>
  T& add() {
    return static_cast<T&>(*_values.emplace_back(new T()).get());
  }

  const JSONValue& operator[](size_t index) const;

  void clear();

  size_t size() const;

  virtual JSONArray* asArray() override;
  virtual const JSONArray* asArray() const override;

  virtual void serialise(std::ostream& os, std::string_view indent = "") const override;
  virtual Type getType() const override;

  static const Type TYPE;

 private:
  std::vector<std::unique_ptr<JSONValue>> _values;
};

class JSONString : public JSONValue, public std::string {
 public:
  JSONString() = default;
  JSONString(std::istream& is, bool* success = nullptr, const JSONToken& first_token = JSONToken());
  virtual ~JSONString() override = default;

  virtual JSONString* asString() override;
  virtual const JSONString* asString() const override;

  virtual void serialise(std::ostream& os, std::string_view indent = "") const override;
  virtual Type getType() const override;

  static const Type TYPE;
};

class JSONNumber : public JSONValue {
 public:
  JSONNumber() = default;
  JSONNumber(std::istream& is, bool* success = nullptr);
  virtual ~JSONNumber() override = default;

  void set(int64_t num);
  void set(uint64_t num);
  void set(double num);

  int64_t getInt64() const;
  uint64_t getUInt64() const;
  double getDouble() const;

  virtual JSONNumber* asNumber() override;
  virtual const JSONNumber* asNumber() const override;

  virtual void serialise(std::ostream& os, std::string_view indent = "") const override;
  virtual Type getType() const override;

  static const Type TYPE;

 private:
  std::variant<int64_t, uint64_t, double> _num = 0;
};

class JSONBool : public JSONValue {
 public:
  JSONBool() = default;
  JSONBool(std::istream& is, bool* success = nullptr);
  virtual ~JSONBool() override = default;

  void set(bool value);
  bool get() const;

  virtual JSONBool* asBool() override;
  virtual const JSONBool* asBool() const override;

  virtual void serialise(std::ostream& os, std::string_view indent = "") const override;
  virtual Type getType() const override;

  static const Type TYPE;

 private:
  bool _value = false;
};

class JSONNull : public JSONValue {
 public:
  JSONNull() = default;
  JSONNull(std::istream& is, bool* success = nullptr);
  virtual ~JSONNull() override = default;

  virtual const JSONNull* asNull() const override;

  virtual void serialise(std::ostream& os, std::string_view indent = "") const override;
  virtual Type getType() const override;

  static const Type TYPE;
};
}  // namespace pancake