#pragma once

#include "ecs/common.hpp"
#include "graphics/buffer_format.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"
#include "util/pstruct.hpp"

namespace pancake {
PSTRUCT(Base)
PSTRUCT_MEMBER_INITIALISED(Entity, self, Entity::null)
PSTRUCT_MEMBER_INITIALISED(Entity, parent, Entity::null)
PSTRUCT_MEMBER_INITIALISED(Entity, first_child, Entity::null)
PSTRUCT_MEMBER_INITIALISED(Entity, next_sibling, Entity::null)
PSTRUCT_MEMBER_INITIALISED(GUID, guid, GUID::null)
PSTRUCT_END()

PSTRUCT(RenderTargetInfo)
PSTRUCT_MEMBER_INITIALISED(BufferFormat, format, BufferFormat::RGBA32F);
PSTRUCT_MEMBER_INITIALISED(Vec4f, clear_colour, Vec4f(0.f, 0.f, 0.f, 1.f));
PSTRUCT_END()

using RenderTargetInfos = RenderTargetInfo[4];

PSTRUCT(FramebufferInfo)
PSTRUCT_MEMBER_INITIALISED(RenderTargetInfos, render_targets, {})
PSTRUCT_MEMBER_INITIALISED(Vec2i, size, Vec2i::zeros())
PSTRUCT_MEMBER_INITIALISED(Vec2f, relative_size, Vec2f::ones())
PSTRUCT_MEMBER_INITIALISED(int, blit_priority, -1)
PSTRUCT_MEMBER_INITIALISED(char, num_targets, 1)
PSTRUCT_MEMBER_INITIALISED(bool, auto_clear, true)
PSTRUCT_MEMBER_INITIALISED(bool, depth_test, true)
PSTRUCT_END()
}  // namespace pancake