#pragma once

#include "util/type_desc_library.hpp"

#include <cstdint>
#include <functional>
#include <iomanip>
#include <sstream>

namespace pancake {
template <int F>
class Bitmask {
 public:
  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using pointer = const int*;
    using reference = const int&;

    Iterator(const uint64_t* fields, int bit = 0) : _fields(fields), _bit(bit) {
      while ((_bit < (F * field_size)) && (!get(_bit))) {
        ++_bit;
      }
    }

    reference operator*() const { return _bit; }
    pointer operator->() { return &_bit; }
    Iterator& operator++() {
      ++_bit;
      while ((_bit < (F * field_size)) && (!get(_bit))) {
        ++_bit;
      }
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& a, const Iterator& b) {
      return (a._fields == b._fields) && (a._bit == b._bit);
    };
    friend bool operator!=(const Iterator& a, const Iterator& b) {
      return (a._fields != b._fields) || (a._bit != b._bit);
    };

   private:
    bool get(int b) const { return _fields[b / field_size] & (1LL << (b % field_size)); }

    const uint64_t* _fields;
    int _bit;
  };

  Bitmask() : _fields() { clear(); }
  Bitmask(const Bitmask<F>& other) : _fields() {
    for (int f = 0; f < F; ++f) {
      _fields[f] = other._fields[f];
    }
  }

  ~Bitmask() {}

  Bitmask<F>& clear() {
    for (int f = 0; f < F; ++f) {
      _fields[f] = 0;
    }
    return *this;
  }

  Bitmask<F>& fill() {
    for (int f = 0; f < F; ++f) {
      _fields[f] = UINT64_MAX;
    }
    return *this;
  }

  Bitmask<F>& unset(int b) {
    _fields[b / field_size] &= ~(1LL << (b % field_size));
    return *this;
  }
  Bitmask<F>& set(int b) {
    _fields[b / field_size] |= (1LL << (b % field_size));
    return *this;
  }
  Bitmask<F>& set(int b, bool v) {
    v ? set(b) : unset(b);
    return *this;
  }

  bool get(int b) const { return _fields[b / field_size] & (1LL << (b % field_size)); }
  bool operator[](int b) const { return get(b); }

  Bitmask<F> with(int b) const { return Bitmask<F>(*this).set(b); }
  Bitmask<F> without(int b) const { return Bitmask<F>(*this).unset(b); }

  void bitAnd(const Bitmask<F>& rhs) {
    for (int f = 0; f < F; ++f) {
      _fields[f] &= rhs._fields[f];
    }
  }

  void bitOr(const Bitmask<F>& rhs) {
    for (int f = 0; f < F; ++f) {
      _fields[f] |= rhs._fields[f];
    }
  }

  std::string bin() const {
    std::string out = "";
    for (int b = no_of_bits - 1; b >= 0; --b) {
      out += get(b) ? "1" : "0";
    }
    return out;
  }

  std::string hex() const {
    std::stringstream ss;
    ss << std::setfill('0');
    for (int f = F - 1; f >= 0; --f) {
      ss << std::setw(8) << std::hex << _fields[f];
    }
    return ss.str();
  }

  static Bitmask<F> from_hex(const std::string& hex) {
    Bitmask<F> mask = Bitmask<F>();
    std::stringstream ss;
    ss << std::hex;
    for (int f = 0; f < F; ++f) {
      size_t end = hex.size() - (f * 8);
      if (end > 0) {
        size_t start = end - 8;
        if (start < 0) {
          start = 0;
        }
        std::string segment = hex.substr(start, end - start);
        ss << segment;
        ss >> mask._fields[f];
      }
    }
    return mask;
  }

  Iterator begin() const { return Iterator(_fields); }
  Iterator end() const { return Iterator(_fields, (F * field_size)); }

  Bitmask<F> operator&(const Bitmask<F>& rhs) const {
    Bitmask<F> mask;
    for (int f = 0; f < F; ++f) {
      mask._fields[f] = _fields[f] & rhs._fields[f];
    }
    return mask;
  }

  Bitmask<F> operator|(const Bitmask<F>& rhs) const {
    Bitmask<F> mask;
    for (int f = 0; f < F; ++f) {
      mask._fields[f] = _fields[f] | rhs._fields[f];
    }
    return mask;
  }

  bool operator<(const Bitmask<F>& rhs) const {
    for (int f = 0; f < F; ++f) {
      if (_fields[f] != rhs._fields[f]) {
        return _fields[f] < rhs._fields[f];
      }
    }
    return false;
  }

  bool operator==(const Bitmask<F>& rhs) const {
    for (int f = 0; f < F; ++f) {
      if (_fields[f] != rhs._fields[f]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Bitmask<F>& rhs) const {
    for (int f = 0; f < F; ++f) {
      if (_fields[f] != rhs._fields[f]) {
        return true;
      }
    }
    return false;
  }

  Bitmask<F>& operator=(const Bitmask<F>& rhs) {
    for (int f = 0; f < F; ++f) {
      _fields[f] = rhs._fields[f];
    }
    return *this;
  }

  static const Bitmask<F>& empty() {
    static const Bitmask<F> mask = Bitmask<F>();
    return mask;
  }

  static const Bitmask<F>& full() {
    static const Bitmask<F> mask = Bitmask<F>().fill();
    return mask;
  }

  static const int field_size = 64;
  static const int no_of_fields = F;
  static const int no_of_bits = F * 64;

 private:
  friend struct std::hash<Bitmask<F>>;

  static inline const TypeDesc& DESC =
      TypeDescLibrary::get<Bitmask<F>>().addField("_fields",
                                                  0,
                                                  TypeDescLibrary::get<uint64_t[F]>());

  uint64_t _fields[F];
};
}  // namespace pancake

template <int F>
struct std::hash<pancake::Bitmask<F>> {
  size_t operator()(const pancake::Bitmask<F>& mask) const noexcept {
    size_t hash = 0;
    for (int i = 0; i < F; ++i) {
      hash ^= (std::hash<uint64_t>{}(mask._fields[i]) << i);
    }
    return hash;
  }
};