#include "ecs/messages.hpp"

#include "util/assert.hpp"

using namespace pancake;

void Messages::add(const TypeDesc& desc, bool global, size_t pair_size, size_t pair_offset) {
  if (!_message_ids.contains(desc)) {
    const MessageId id = static_cast<MessageId>(_message_ids.size());
    _message_ids.emplace(desc, id);
    _message_infos.emplace_back(desc, id, global, pair_size, pair_offset);
  } else {
    ensure(false);
  }
}

MessageId Messages::_getId(const TypeDesc& desc) const {
  ensure(_message_ids.contains(desc));
  return _message_ids.at(desc);
}

bool Messages::_isMessage(const TypeDesc& desc) const {
  return _message_ids.contains(desc);
}

const TypeDesc& Messages::_getDesc(MessageId msg_id) const {
  ensure(msg_id < _message_ids.size());
  return _message_infos[msg_id].desc;
}

bool Messages::_isGlobal(MessageId msg_id) const {
  ensure(msg_id < _message_ids.size());
  return _message_infos[msg_id].global;
}

MessageId Messages::getId(const TypeDesc& desc) {
  return Messages::get()._getId(desc);
}

bool Messages::isMessage(const TypeDesc& desc) {
  return get()._isMessage(desc);
}

const TypeDesc& Messages::getDesc(MessageId msg_id) {
  return get()._getDesc(msg_id);
}

bool Messages::isGlobal(MessageId msg_id) {
  return get()._isGlobal(msg_id);
}

std::span<const MessageInfo> Messages::getMessageInfos() {
  return get()._message_infos;
}

Messages& Messages::get() {
  static Messages Messages;
  return Messages;
}