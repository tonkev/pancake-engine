#pragma once

#include "ecs/common.hpp"
#include "ecs/message_access.hpp"
#include "ecs/message_board.hpp"
#include "util/bitmask.hpp"
#include "util/type_desc_library.hpp"

#include <span>
#include <unordered_map>
#include <vector>

namespace pancake {
struct MessageInfo {
  const TypeDesc& desc;
  MessageId id;
  bool global;
  size_t pair_size;
  size_t pair_offset;
};

class Messages {
 public:
  template <typename T, bool global = false>
  class StaticAdder {
   public:
    StaticAdder() {
      MessageBoard::MessagePair<T> pair;
      Messages::get().add(TypeDescLibrary::get<T>(), global, sizeof(MessageBoard::MessagePair<T>),
                          ((size_t)&pair.message()) - ((size_t)&pair));
    }
  };

  template <typename... Ts>
  static MessageMask getMask() {
    MessageMask mask = MessageMask();
    const MessageId ids[] = {getId(TypeDescLibrary::get<Ts>())...};
    for (const MessageId id : ids) {
      mask.set(id);
    }
    return mask;
  }

  template <typename... Ts>
  static MessageAccess getAccess() {
    const TypeDesc* recv_descs[] = {std::is_const_v<Ts> ? &TypeDescLibrary::get<Ts>() : nullptr...};
    const TypeDesc* send_descs[] = {(!std::is_const_v<Ts>) ? &TypeDescLibrary::get<Ts>()
                                                           : nullptr...};

    MessageMask recv_mask = MessageMask();
    for (const TypeDesc* recv_desc : recv_descs) {
      if (nullptr == recv_desc) {
        continue;
      }
      recv_mask.set(getId(*recv_desc));
    }

    MessageMask send_mask = MessageMask();
    for (const TypeDesc* send_desc : send_descs) {
      if (nullptr == send_desc) {
        continue;
      }
      send_mask.set(getId(*send_desc));
    }

    return MessageAccess(recv_mask, send_mask);
  }

  static MessageId getId(const TypeDesc& desc);
  static bool isMessage(const TypeDesc& desc);

  static const TypeDesc& getDesc(MessageId msg_id);
  static bool isGlobal(MessageId msg_id);

  static std::span<const MessageInfo> getMessageInfos();

 protected:
  void add(const TypeDesc& desc, bool global, size_t pair_size, size_t pair_offset);

  MessageId _getId(const TypeDesc& desc) const;
  bool _isMessage(const TypeDesc& desc) const;

  const TypeDesc& _getDesc(ComponentId msg_id) const;
  bool _isGlobal(MessageId msg_id) const;

  template <typename T, bool global>
  friend class StaticAdder;

  static Messages& get();

 private:
  Messages() = default;

  using MessageIds = std::unordered_map<std::reference_wrapper<const TypeDesc>,
                                        MessageId,
                                        std::hash<TypeDesc>,
                                        std::equal_to<TypeDesc>>;

  std::vector<MessageInfo> _message_infos;
  MessageIds _message_ids;
};
}  // namespace pancake