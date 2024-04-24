#include "util/type_id.hpp"

using namespace pancake;

bool TypeId::operator==(const TypeId& rhs) const {
  return this == &rhs;
}

size_t std::hash<pancake::TypeId>::operator()(const pancake::TypeId& tid) const noexcept {
  return std::hash<const void*>{}(&tid);
};