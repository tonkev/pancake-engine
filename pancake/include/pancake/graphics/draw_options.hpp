#pragma once

namespace pancake {
struct DrawOptions {
  bool depth_test = true;

  auto operator<=>(const DrawOptions&) const = default;
  bool operator==(const DrawOptions&) const = default;
};
}  // namespace pancake