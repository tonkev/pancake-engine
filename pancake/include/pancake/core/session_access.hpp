#pragma once

#include <compare>

namespace pancake {
class SessionAccess {
 public:
  SessionAccess();

  SessionAccess& addImGui();
  SessionAccess& addRenderer();
  SessionAccess& addResources();

  bool intersects(const SessionAccess& other) const;

  bool hasImGuiAccess() const;
  bool hasRendererAccess() const;
  bool hasResourcesAccess() const;

  SessionAccess operator&(const SessionAccess& rhs) const;
  SessionAccess operator|(const SessionAccess& rhs) const;

  auto operator<=>(const SessionAccess&) const = default;

 private:
  bool _imGui;
  bool _renderer;
  bool _resources;
};
}  // namespace pancake