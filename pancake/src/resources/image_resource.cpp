#include "resources/image_resource.hpp"

#include "resources/resources.hpp"
#include "util/fewi.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <filesystem>
#include <fstream>

using namespace pancake;
namespace fs = std::filesystem;

Resources::Creator<ImageResource> image_resource_creator;

const Resource::Type ImageResource::TYPE = Type::Image;

ImageResource::ImageResource(std::string_view path, const GUID& guid)
    : Resource(path, guid), _data(nullptr), _size(0, 0), _channels(0) {}

ImageResource::ImageResource(std::string_view path,
                             int width,
                             int height,
                             int channels,
                             const GUID& guid)
    : Resource(path, guid),
      _data(new unsigned char[width * height * channels]),
      _size(width, height),
      _channels(channels),
      _data_deleter([](unsigned char* ptr) { delete[] ptr; }) {}

ImageResource::~ImageResource() {}

void ImageResource::_load() {
  if (nullptr != _data) {
    _data_deleter(_data);
    _data = nullptr;
  }
  _data = stbi_load(_full_path.data(), &_size.x(), &_size.y(), &_channels, 4);
  if (nullptr == _data) {
    FEWI::error("Failed to load image " + _full_path + " ! : " + stbi_failure_reason());
  }
  _data_deleter = stbi_image_free;
  updated();
}

void ImageResource::_save() {
  std::string_view ext = extension();
  if ("png" == ext) {
    stbi_write_png(_full_path.data(), _size.x(), _size.y(), _channels, _data,
                   _size.x() * _channels);
  } else if ("bmp" == ext) {
    stbi_write_bmp(_full_path.data(), _size.x(), _size.y(), _channels, _data);
  } else if ("tga" == ext) {
    stbi_write_tga(_full_path.data(), _size.x(), _size.y(), _channels, _data);
  } else if ("jpg" == ext) {
    stbi_write_jpg(_full_path.data(), _size.x(), _size.y(), _channels, _data, 100);
  } else {
    FEWI::error(std::string("extension ") + ext.data() + " unsupported for saving image!");
  }
}

void ImageResource::fill(char r, char g, char b, char a) {
  for (int x = 0; x < _size.x(); ++x) {
    for (int y = 0; y < _size.x(); ++y) {
      if (0 < _channels) {
        _data[(((y * _size.x()) + x) * _channels) + 0] = r;
      }
      if (1 < _channels) {
        _data[(((y * _size.x()) + x) * _channels) + 1] = g;
      }
      if (2 < _channels) {
        _data[(((y * _size.x()) + x) * _channels) + 2] = b;
      }
      if (3 < _channels) {
        _data[(((y * _size.x()) + x) * _channels) + 3] = a;
      }
    }
  }
  updated();
}

void ImageResource::blit(const ImageResource& res, int x, int y) {
  blit(res, x, y, 0, 0, res.width(), res.height());
}

void ImageResource::blit(const ImageResource& res,
                         int dst_x,
                         int dst_y,
                         int src_x,
                         int src_y,
                         int src_w,
                         int src_h) {
  const unsigned char* res_data = res.data();
  const int res_width = res.width();
  const int res_height = res.height();
  const int res_channels = res.channels();

  const int i_max = std::min(src_x + src_w, res_width);
  const int j_max = std::min(src_y + src_h, res_height);

  for (int i = src_x; i < i_max; ++i) {
    int ix = dst_x + i;
    if (ix < 0) {
      continue;
    }
    if (_size.x() < ix) {
      break;
    }

    for (int j = 0; j < j_max; ++j) {
      int jy = j + dst_y;
      if (jy < 0) {
        continue;
      }
      if (_size.y() < jy) {
        break;
      }

      for (int c = 0; (c < _channels) && (c < res_channels); ++c) {
        _data[(((jy * _size.x()) + ix) * _channels) + c] =
            res_data[(((j * res_width) + i) * res_channels) + c];
      }

      for (int c = res_channels; c < _channels; ++c) {
        _data[(((jy * _size.y()) + ix) * _channels) + c] = 255;
      }
    }
  }
  updated();
}

void ImageResource::swapContent(ImageResource& res) {
  std::swap(_data, res._data);
  std::swap(_size, res._size);
  std::swap(_channels, res._channels);
  std::swap(_data_deleter, res._data_deleter);
  updated();
  res.updated();
}

const unsigned char* ImageResource::data() const {
  return _data;
}

const Vec2i& ImageResource::size() const {
  return _size;
}

int ImageResource::width() const {
  return _size.x();
}

int ImageResource::height() const {
  return _size.y();
}

int ImageResource::channels() const {
  return _channels;
}

Resource::Type ImageResource::type() const {
  return Type::Image;
}