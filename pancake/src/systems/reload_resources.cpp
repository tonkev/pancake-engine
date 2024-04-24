#include "systems/reload_resources.hpp"

#include "core/input.hpp"
#include "core/session.hpp"
#include "core/session_access.hpp"
#include "core/session_wrapper.hpp"
#include "resources/resource.hpp"

#include <filesystem>

using namespace pancake;

const LogicSystem::StaticAdder<ReloadResources> reload_resources_adder{};

void ReloadResources::_run(const SessionWrapper& session) const {
  if (session.input().isKeyJustPressed(KBCode::F5)) {
    for (auto& [_, res] : session.resources().getResources()) {
      if (std::filesystem::exists(res->fullPath())) {
        res->load();
      }
    }
  }
}

std::string_view ReloadResources::name() const {
  return "ReloadResources";
}

SystemId ReloadResources::id() const {
  return System::id<ReloadResources>();
}

const SessionAccess& ReloadResources::getSessionAccess() const {
  static const SessionAccess session_access = SessionAccess().addResources();
  return session_access;
}
