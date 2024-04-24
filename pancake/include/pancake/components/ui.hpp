#pragma once

#include "components/common.hpp"
#include "ecs/dynamic_buffer.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"
#include "util/pstruct.hpp"

namespace pancake {
PSTRUCT(UIContainer)
enum class AnchoringType { Start, Centre, End };
enum class PositioningType { FixedFromStart, FixedFromCentre, FixedFromEnd, Relative };
enum class SizingType { Fixed, Relative };

AnchoringType getAnchoringType(int axis) const;
PositioningType getPositioningType(int axis) const;
SizingType getSizingType(int axis) const;

PSTRUCT_MEMBER_INITIALISED(Vec2f, position, Vec2f::zeros())
PSTRUCT_MEMBER_INITIALISED(Vec2f, size, Vec2f::ones() * 10.f)
PSTRUCT_MEMBER_INITIALISED(Vec2f, absolute_position, Vec2f::zeros())
PSTRUCT_MEMBER_INITIALISED(Vec2f, absolute_size, Vec2f::ones() * 10.f)
PSTRUCT_MEMBER_INITIALISED(AnchoringType, anchoring_x, AnchoringType::Start)
PSTRUCT_MEMBER_INITIALISED(AnchoringType, anchoring_y, AnchoringType::Start)
PSTRUCT_MEMBER_INITIALISED(PositioningType, positioning_x, PositioningType::FixedFromStart)
PSTRUCT_MEMBER_INITIALISED(PositioningType, positioning_y, PositioningType::FixedFromStart)
PSTRUCT_MEMBER_INITIALISED(SizingType, sizing_x, SizingType::Fixed)
PSTRUCT_MEMBER_INITIALISED(SizingType, sizing_y, SizingType::Fixed)
PSTRUCT_END()

PSTRUCT(UIBackground)
PSTRUCT_MEMBER_INITIALISED(Vec4f, colour, Vec4f::ones())
PSTRUCT_END()

using UIString = DynamicBuffer<char, 10>;

PSTRUCT(UIText)
PSTRUCT_MEMBER_INITIALISED(Vec2f, font_size, Vec2f(20.f));
PSTRUCT_MEMBER_INITIALISED(GUID, font, GUID::null);
PSTRUCT_MEMBER_INITIALISED(bool, wraparound, true);
PSTRUCT_MEMBER_INITIALISED(bool, cutoff, true);
PSTRUCT_END()

PSTRUCT(Text)
PSTRUCT_MEMBER_INITIALISED(CameraMask, camera_mask, CameraMask::empty().with(0));
PSTRUCT_MEMBER_INITIALISED(Vec2f, font_size, Vec2f(20.f, 20.f))
PSTRUCT_MEMBER_INITIALISED(GUID, font, GUID::null)
PSTRUCT_MEMBER_INITIALISED(GUID, text, GUID::null)
PSTRUCT_END()
}  // namespace pancake