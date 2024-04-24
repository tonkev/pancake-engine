#pragma once

#include "util/type_desc.hpp"

#include <span>

namespace pancake {
namespace default_components {
std::span<std::reference_wrapper<const TypeDesc>> get();
}
}  // namespace pancake