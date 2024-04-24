#include "core/session_access.hpp"

using namespace pancake;

SessionAccess::SessionAccess() : _imGui(false), _renderer(false), _resources(false) {}

SessionAccess& SessionAccess::addImGui() {
  _imGui = true;
  return *this;
}

SessionAccess& SessionAccess::addRenderer() {
  _renderer = true;
  return *this;
}

SessionAccess& SessionAccess::addResources() {
  _resources = true;
  return *this;
}

bool SessionAccess::intersects(const SessionAccess& other) const {
  return (_imGui && other._imGui) || (_renderer && other._renderer) ||
         (_resources && other._resources);
}

bool SessionAccess::hasImGuiAccess() const {
  return _imGui;
}

bool SessionAccess::hasRendererAccess() const {
  return _renderer;
}

bool SessionAccess::hasResourcesAccess() const {
  return _resources;
}

SessionAccess SessionAccess::operator&(const SessionAccess& rhs) const {
  SessionAccess access;
  access._renderer = _imGui && rhs._imGui;
  access._renderer = _renderer && rhs._renderer;
  access._resources = _resources && rhs._resources;
  return access;
}

SessionAccess SessionAccess::operator|(const SessionAccess& rhs) const {
  SessionAccess access;
  access._imGui = _imGui || rhs._imGui;
  access._renderer = _renderer || rhs._renderer;
  access._resources = _resources || rhs._resources;
  return access;
}
