#pragma once

#include "ecs/common.hpp"
#include "util/pstruct.hpp"

namespace pancake {
PSTRUCT(UIOnHover)
PSTRUCT_MEMBER_INITIALISED(Entity, entity, Entity::null);
PSTRUCT_END()

PSTRUCT(UIOnClick)
PSTRUCT_MEMBER_INITIALISED(Entity, entity, Entity::null);
PSTRUCT_END()
}  // namespace pancake