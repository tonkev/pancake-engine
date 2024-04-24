#pragma once

#include "ecs/system.hpp"
#include "pancake.hpp"

#include <set>

namespace pancake {
class Session;
class LogicSystem : public System {
 public:
  using Creators = std::set<LogicSystem* (*)(Session&)>;

  template <typename T>
  class StaticAdder {
   private:
    static LogicSystem* creator(Session& session) { return new T(session); }

   public:
    StaticAdder() { getCreators().emplace(creator); }
  };

  virtual ~LogicSystem() = default;

  static Ptr<Systems> createLogicSystems(Session& session);

 protected:
  using System::System;
  static Creators& getCreators();

  template <typename T>
  friend class StaticAdder;
};
}  // namespace pancake