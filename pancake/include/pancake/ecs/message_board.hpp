#pragma once

#include "ecs/common.hpp"
#include "util/assert.hpp"
#include "util/type_desc_library.hpp"

#include <span>
#include <vector>

namespace pancake {
class TypeDesc;
struct MessageInfo;

class MessageBoard {
 public:
  MessageBoard(const MessageInfo& info);

  template <typename T>
  void add(const T& message, SystemNodeId sender) {
    ensure(TypeDescLibrary::get<T>() == _desc);
    _add(&message, sender);
  }

  template <typename T>
  struct MessagePair {
   public:
    const T& message() const { return _message; }

    operator const T&() const { return _message; }

   private:
    SystemNodeId _sender;
    T _message;
  };

  template <typename T>
  std::span<const MessagePair<T>> getMessages() const {
    ensure(TypeDescLibrary::get<T>() == _desc);
    return std::span(reinterpret_cast<const MessagePair<T>*>(_pool.data()), _pool.size() / _stride);
  }

  void clearFrom(SystemNodeId sender);
  void clear();

 private:
  void _add(const void* message, SystemNodeId sender);

  const TypeDesc& _desc;
  const size_t _offset;
  const size_t _stride;
  std::vector<char> _pool;
};
}  // namespace pancake