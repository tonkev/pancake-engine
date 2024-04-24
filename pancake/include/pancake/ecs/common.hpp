#pragma once

#include "util/bitmask.hpp"
#include "util/generational_index.hpp"

#include <cstdint>

namespace pancake {
using Entity = GenerationalIndex<uint64_t>;

using ArchetypeId = uint32_t;
using ComponentId = uint32_t;
using EncompasserId = uint32_t;
using MessageId = uint32_t;
using SystemNodeId = uint32_t;

using ComponentMask = Bitmask<4>;
using EncompasserMask = Bitmask<4>;
using MessageMask = Bitmask<4>;
}  // namespace pancake