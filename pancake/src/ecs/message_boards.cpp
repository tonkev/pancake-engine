#include "ecs/message_boards.hpp"

#include "ecs/messages.hpp"

using namespace pancake;

MessageBoards::MessageBoards(bool is_global) {
  const auto message_infos = Messages::getMessageInfos();
  _boards.reserve(message_infos.size());

  for (const MessageInfo& mi : Messages::getMessageInfos()) {
    _boards.emplace_back((mi.global == is_global) ? new MessageBoard(mi) : nullptr);
  }
}

MessageBoard& MessageBoards::getMessageBoard(MessageId message_id) {
  ensure((message_id < _boards.size()) && _boards[message_id]);
  return *_boards[message_id];
}

void MessageBoards::clear() {
  for (auto& board : _boards) {
    board->clear();
  }
}