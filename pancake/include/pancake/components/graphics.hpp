#pragma once

#include "ecs/common.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"
#include "util/pstruct.hpp"

namespace pancake {
PSTRUCT(MaterialInstance)
PSTRUCT_MEMBER_INITIALISED(GUID, material, GUID::null)
PSTRUCT_END()

PSTRUCT(PointLight)
PSTRUCT_MEMBER_INITIALISED(Vec4f, color, Vec4f::ones())
PSTRUCT_END()
}  // namespace pancake