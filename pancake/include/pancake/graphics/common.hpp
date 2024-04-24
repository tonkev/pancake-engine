#pragma once

#include <optional>

namespace pancake {
class Framebuffer;
class Material;
class Texture;
class Tileset;

using OptFramebufferConstRef = std::optional<std::reference_wrapper<const Framebuffer>>;
using OptMaterialConstRef = std::optional<std::reference_wrapper<const Material>>;
using OptTextureConstRef = std::optional<std::reference_wrapper<const Texture>>;
using OptTilesetConstRef = std::optional<std::reference_wrapper<const Tileset>>;
}  // namespace pancake