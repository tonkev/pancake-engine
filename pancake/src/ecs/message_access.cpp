#include "ecs/message_access.hpp"

#include "ecs/messages.hpp"

using namespace pancake;

const MessageAccess MessageAccess::empty{};

const MessageAccess MessageAccess::global = [] {
  MessageAccess access;

  for (const MessageInfo& mi : Messages::getMessageInfos()) {
    if (mi.global) {
      access._receives.set(mi.id);
      access._sends.set(mi.id);
    }
  }

  return access;
}();

const MessageAccess MessageAccess::local = [] {
  MessageAccess access;

  for (const MessageInfo& mi : Messages::getMessageInfos()) {
    if (!mi.global) {
      access._receives.set(mi.id);
      access._sends.set(mi.id);
    }
  }

  return access;
}();

MessageAccess::MessageAccess(const MessageMask& receives, const MessageMask& sends)
    : _receives(receives), _sends(sends) {}

const MessageMask& MessageAccess::getReceives() const {
  return _receives;
}

const MessageMask& MessageAccess::getSends() const {
  return _sends;
}

bool MessageAccess::intersects(const MessageAccess& other) const {
  return ((_receives & other._sends) != MessageMask::empty()) ||
         ((other._receives & _sends) != MessageMask::empty()) ||
         ((_sends & other._sends) != MessageMask::empty());
}

bool MessageAccess::subsets(const MessageAccess& super_set) const {
  return ((_receives | super_set._receives) == super_set._receives) &&
         ((_sends | super_set._sends) == super_set._sends);
}

MessageAccess MessageAccess::operator&(const MessageAccess& rhs) const {
  return MessageAccess(_receives & rhs._receives, _sends & rhs._sends);
}

MessageAccess MessageAccess::operator|(const MessageAccess& rhs) const {
  return MessageAccess(_receives | rhs._receives, _sends | rhs._sends);
}
