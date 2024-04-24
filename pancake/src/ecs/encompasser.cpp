#include "ecs/encompasser.hpp"

using namespace pancake;

Encompasser::Encompasser(const GUID& guid) : _guid(guid) {}

const GUID& Encompasser::guid() const {
  return _guid;
}