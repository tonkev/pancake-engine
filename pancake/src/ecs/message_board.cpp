#include "ecs/message_board.hpp"

#include "ecs/messages.hpp"

#include <cstring>

using namespace pancake;

MessageBoard::MessageBoard(const MessageInfo& info)
    : _desc(info.desc), _offset(info.pair_offset), _stride(info.pair_size) {}

void MessageBoard::_add(const void* message, SystemNodeId sender) {
  const size_t msg_size = _desc.size();
  const size_t old_size = _pool.size();
  _pool.resize(old_size + _stride);
  std::memcpy(&_pool[old_size], &sender, sizeof(SystemNodeId));
  std::memcpy(&_pool[old_size + _offset], message, msg_size);
}

void MessageBoard::clearFrom(SystemNodeId sender) {
  for (size_t i = _pool.size() - _stride; i < _pool.size(); i -= _stride) {
    if (*reinterpret_cast<SystemNodeId*>(&_pool[i]) == sender) {
      if ((i + _stride) < _pool.size()) {
        std::memcpy(&_pool[i], &_pool[i + _stride], _pool.size() - (i + _stride));
      }
      _pool.resize(i);
    }
  }
}

void MessageBoard::clear() {
  _pool.clear();
}