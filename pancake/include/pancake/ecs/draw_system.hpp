#pragma once

#include "ecs/system.hpp"
#include "pancake.hpp"

#include <set>

namespace pancake {
class Session;
class DrawSystem : public System {
 public:
  using Creators = std::set<DrawSystem* (*)(Session&)>;

  template <typename T>
  class StaticAdder {
   private:
    static DrawSystem* creator(Session& session) { return new T(session); }

   public:
    StaticAdder() { getCreators().emplace(creator); }
  };

  virtual ~DrawSystem() = default;

  static Ptr<Systems> createDrawSystems(Session& session);

 protected:
  using System::System;
  static Creators& getCreators();

  template <typename T>
  friend class StaticAdder;

 private:
  static Creators _creators;
};
}  // namespace pancake