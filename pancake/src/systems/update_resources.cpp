#include "systems/update_resources.hpp"

#include "core/input.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "resources/resource.hpp"

#include <filesystem>

using namespace pancake;

const LogicSystem::StaticAdder<UpdateResources> update_resources_adder{};

void UpdateResources::_run(const SessionWrapper& session) const {
  Resources& resources = session.resources();
  for (auto& [_, res] : resources.getResources()) {
    res->ensureUpdated(resources);
  }
}

std::string_view UpdateResources::name() const {
  return "UpdateResources";
}

SystemId UpdateResources::id() const {
  return System::id<UpdateResources>();
}

const SessionAccess& UpdateResources::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addResources();
  return session_access;
}
