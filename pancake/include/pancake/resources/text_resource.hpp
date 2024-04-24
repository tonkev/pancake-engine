#pragma once

#include "resources/resource.hpp"

namespace pancake {
class TextResource : public Resource {
 public:
  TextResource(std::string_view path, const GUID& guid);
  virtual ~TextResource() = default;

  std::string& text();
  std::string_view constText() const;

  virtual Type type() const override;

  static const Type TYPE;

 protected:
  virtual void _load() override;
  virtual void _save() override;

  std::string _text;
};
}  // namespace pancake