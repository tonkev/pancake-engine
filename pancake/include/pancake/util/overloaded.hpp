#pragma once

namespace pancake {
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
}  // namespace pancake