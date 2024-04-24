#pragma once

#include "resources/resource.hpp"
#include "util/matrix.hpp"

namespace pancake {
class ImageResource : public Resource {
 public:
  ImageResource(std::string_view path, const GUID& guid);
  ImageResource(std::string_view path, int width, int height, int channels, const GUID& guid);
  virtual ~ImageResource();

  void fill(char r, char g, char b, char a);

  void blit(const ImageResource& res, int x, int y);
  void
  blit(const ImageResource& res, int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h);
  void swapContent(ImageResource& res);

  const unsigned char* data() const;
  const Vec2i& size() const;
  int width() const;
  int height() const;
  int channels() const;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

 private:
  unsigned char* _data;
  Vec2i _size;
  int _channels;
  std::function<void(unsigned char*)> _data_deleter;
};
}  // namespace pancake