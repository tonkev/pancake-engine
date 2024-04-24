#pragma once

#include "ecs/common.hpp"
#include "ecs/message_board.hpp"

#include <memory>
#include <vector>

namespace pancake {
class MessageBoard;
class MessageBoards {
 public:
  MessageBoards(bool is_global);

  MessageBoard& getMessageBoard(MessageId message_id);

  void clear();

 private:
  std::vector<std::unique_ptr<MessageBoard>> _boards;
};
}  // namespace pancake