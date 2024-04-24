#pragma once

#include "pancake.hpp"
#include "resources/image_resource.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"

#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace pancake {
class Resources;
class ImageAtlas {
 public:
  ImageAtlas(std::string_view path, int width, int height, int channels);

  struct Rect {
    Vec2i position;
    Vec2i size;
    uint64_t gen;
  };

  virtual ~ImageAtlas() = default;

  void add(const GUID& image_guid);
  void add(const ImageResource& image);
  std::optional<std::reference_wrapper<const ImageAtlas::Rect>> getRect(const GUID& guid) const;

  void update(Resources& resources);

  ImageResource& image();
  const Vec2i& size() const;
  uint64_t gen() const;

 private:
  ImageResource _underlying_image;
  std::unordered_set<GUID> _pending_images;
  std::unordered_map<GUID, Rect> _packed_rects;
};
}  // namespace pancake
