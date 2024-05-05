#include "graphics/image_atlas.hpp"

#include "resources/resources.hpp"
#include "util/fewi.hpp"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#include <vector>

using namespace pancake;

ImageAtlas::ImageAtlas(std::string_view path, int width, int height, int channels)
    : _underlying_image(path, width, height, channels, GUID::null) {}

void ImageAtlas::add(const GUID& image_guid) {
  if (!_packed_rects.contains(image_guid)) {
    _pending_images.emplace(image_guid);
    Rect& rect = _packed_rects[image_guid];
    rect.gen = std::numeric_limits<uint64_t>::max();
  }
}

void ImageAtlas::add(const ImageResource& image) {
  const GUID& guid = image.guid();

  if (_packed_rects.contains(guid)) {
    Rect& rect = _packed_rects.at(guid);
    if (image.size() < rect.size) {
      _underlying_image.blit(image, rect.position.x(), rect.position.y());
    } else {
      _pending_images.emplace(guid);
    }
    rect.size = image.size();
    rect.gen = image.gen();
  } else {
    _pending_images.emplace(guid);
    Rect& rect = _packed_rects[guid];
    rect.size = image.size();
    rect.gen = image.gen();
  }
}

std::optional<std::reference_wrapper<const ImageAtlas::Rect>> ImageAtlas::getRect(
    const GUID& guid) const {
  std::optional<std::reference_wrapper<const Rect>> rect_opt = std::nullopt;
  if (const auto it = _packed_rects.find(guid); it != _packed_rects.end()) {
    rect_opt = _packed_rects.at(guid);
  }
  return rect_opt;
}

const std::unordered_map<GUID, ImageAtlas::Rect>& ImageAtlas::getRects() const {
  return _packed_rects;
}

const std::unordered_set<GUID>& ImageAtlas::getRejectedImages() const {
  return _rejected_images;
}

void ImageAtlas::update(Resources& resources) {
  _rejected_images.clear();

  for (const auto& [guid, rect] : _packed_rects) {
    std::optional<std::reference_wrapper<ImageResource>> img_res =
        resources.getOrCreate<ImageResource>(guid);
    if (img_res.has_value() && (rect.gen != img_res.value().get().gen())) {
      add(img_res.value());
    }
  }

  if (_pending_images.empty()) {
    return;
  }

  std::vector<GUID> guids;
  for (const auto& [guid, _] : _packed_rects) {
    guids.emplace_back(guid);
  }

  std::vector<stbrp_rect> s_rects;
  for (size_t i = 0; i < guids.size(); ++i) {
    const Rect& rect = _packed_rects.at(guids[i]);
    stbrp_rect& s_rect = s_rects.emplace_back();
    s_rect.id = i;
    s_rect.w = rect.size.x();
    s_rect.h = rect.size.y();
    s_rect.was_packed = false;
  }

  stbrp_context context;
  std::vector<struct stbrp_node> nodes(_packed_rects.size());
  stbrp_init_target(&context, _underlying_image.width(), _underlying_image.height(), nodes.data(),
                    nodes.size());
  int result = stbrp_pack_rects(&context, s_rects.data(), static_cast<int>(s_rects.size()));
  if (result != 1) {
    FEWI::warn() << "ImageAtlas " << _underlying_image.path() << " failed to pack all images!";
  }

  ImageResource old("", _underlying_image.width(), _underlying_image.height(),
                    _underlying_image.channels(), GUID::null);
  _underlying_image.swapContent(old);

  for (const stbrp_rect& s_rect : s_rects) {
    const GUID& guid = guids[s_rect.id];
    Rect& rect = _packed_rects[guid];

    if (s_rect.was_packed) {
      if (_pending_images.contains(guid)) {
        std::optional<std::reference_wrapper<ImageResource>> img_res =
            resources.getOrCreate<ImageResource>(guid);
        if (img_res.has_value()) {
          _underlying_image.blit(img_res.value(), s_rect.x, s_rect.y);
        }
      } else {
        _underlying_image.blit(old, s_rect.x, s_rect.y, rect.position.x(), rect.position.y(),
                               rect.size.x(), rect.size.y());
      }
    } else {
      _packed_rects.erase(guid);
      _rejected_images.insert(guid);
    }

    rect.position.x() = s_rect.x;
    rect.position.y() = s_rect.y;
  }

  _pending_images.clear();
}

ImageResource& ImageAtlas::image() {
  return _underlying_image;
}

const Vec2i& ImageAtlas::size() const {
  return _underlying_image.size();
}

uint64_t ImageAtlas::gen() const {
  return _underlying_image.gen();
}