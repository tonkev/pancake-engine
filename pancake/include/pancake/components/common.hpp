#pragma once

#include "util/bitmask.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"
#include "util/pstruct.hpp"

namespace pancake {
using CameraMask = Bitmask<1>;
using ColliderMask = Bitmask<1>;

PSTRUCT(TextureRef)
TextureRef() = default;
TextureRef(const GUID& texture, int tile);

PSTRUCT_MEMBER_INITIALISED(GUID, texture, GUID::null)
PSTRUCT_MEMBER_INITIALISED(int, tile, -1);

auto operator<=>(const TextureRef&) const = default;
PSTRUCT_END()
}  // namespace pancake