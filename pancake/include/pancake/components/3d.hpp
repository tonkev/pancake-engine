#pragma once

#include "components/common.hpp"
#include "components/graphics.hpp"
#include "components/transform_3d.hpp"
#include "util/matrix.hpp"

namespace pancake {
PSTRUCT(Camera3D)
PSTRUCT_MEMBER_INITIALISED(CameraMask, mask, CameraMask().with(0))
PSTRUCT_MEMBER_INITIALISED(GUID, framebuffer, GUID::null)
PSTRUCT_MEMBER_INITIALISED(float, fov, 90.f)
PSTRUCT_MEMBER_INITIALISED(float, near, 0.01f)
PSTRUCT_MEMBER_INITIALISED(float, far, 1000.f)
PSTRUCT_MEMBER_INITIALISED(bool, perspective, true)
PSTRUCT_END()

PSTRUCT(MeshInstance)
PSTRUCT_MEMBER_INITIALISED(GUID, mesh, GUID::null)
PSTRUCT_MEMBER_INITIALISED(CameraMask, camera_mask, CameraMask().with(0))
PSTRUCT_END()
}  // namespace pancake