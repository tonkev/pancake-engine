#pragma once

#include "components/common.hpp"
#include "components/transform_2d.hpp"
#include "ecs/common.hpp"
#include "ecs/dynamic_buffer.hpp"
#include "util/matrix.hpp"
#include "util/pstruct.hpp"

namespace pancake {
PSTRUCT(Sprite2D)
PSTRUCT_MEMBER(TextureRef, texture)
PSTRUCT_MEMBER_INITIALISED(CameraMask, camera_mask, CameraMask().with(0))
PSTRUCT_END()

PSTRUCT(Camera2D)
PSTRUCT_MEMBER_INITIALISED(CameraMask, mask, CameraMask().with(0))
PSTRUCT_MEMBER_INITIALISED(GUID, framebuffer, GUID::null)
PSTRUCT_END()

PSTRUCT(StaticBody2D)
PSTRUCT_END()

PSTRUCT(KinematicBody2D)
PSTRUCT_MEMBER_INITIALISED(Vec2f, velocity, Vec2f::zeros())
PSTRUCT_END()

PSTRUCT(RectangleCollider2D)
PSTRUCT_MEMBER_INITIALISED(Vec2f, extents, Vec2f(0.5f))
PSTRUCT_MEMBER_INITIALISED(ColliderMask, collider_mask, ColliderMask().with(0))
PSTRUCT_END()

using Points2D = DynamicBuffer<Vec2f, 4>;

PSTRUCT(LineRenderer2D)
PSTRUCT_MEMBER_INITIALISED(float, width, 0.1f)
PSTRUCT_MEMBER_INITIALISED(Vec4f, colour, Vec4f(1.f, 0.f, 0.f, 1.f))
PSTRUCT_MEMBER_INITIALISED(CameraMask, camera_mask, CameraMask().with(0))
PSTRUCT_END()
}  // namespace pancake