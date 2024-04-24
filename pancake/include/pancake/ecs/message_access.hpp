#pragma once

#include "ecs/common.hpp"

namespace pancake {
class MessageAccess {
 public:
  MessageAccess(const MessageMask& receives = MessageMask(),
                const MessageMask& sends = MessageMask());

  const MessageMask& getReceives() const;
  const MessageMask& getSends() const;

  bool intersects(const MessageAccess& other) const;
  bool subsets(const MessageAccess& super_set) const;

  MessageAccess operator&(const MessageAccess& rhs) const;
  MessageAccess operator|(const MessageAccess& rhs) const;

  auto operator<=>(const MessageAccess&) const = default;

  static const MessageAccess empty;
  static const MessageAccess global;
  static const MessageAccess local;

 private:
  MessageMask _receives;
  MessageMask _sends;
};
}  // namespace pancake