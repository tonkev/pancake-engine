#include "resources/texture_props_resource.hpp"

#include "resources/resources.hpp"
#include "util/componentify_json.hpp"
#include "util/jsonify_component.hpp"

using namespace pancake;

Resources::Creator<TexturePropsResource> texture_props_resource_creator;

const Resource::Type TexturePropsResource::TYPE = Type::TextureProps;

TexturePropsResource::TexturePropsResource(std::string_view path, const GUID& guid)
    : JSONResource(path, guid) {}

void TexturePropsResource::_load() {
  JSONResource::_load();
  if (const JSONValue* val = _json.get("filter"); nullptr != val) {
    TypeDescLibrary::get<TextureFilter>().visit(ComponentifyJSON(&_filter, *val));
  }
  if (const JSONValue* val = _json.get("format"); nullptr != val) {
    TypeDescLibrary::get<BufferFormat>().visit(ComponentifyJSON(&_format, *val));
  }
  if (const JSONValue* val = _json.get("size"); nullptr != val) {
    TypeDescLibrary::get<Vec2i>().visit(ComponentifyJSON(&_size, *val));
  }
  if (const JSONValue* val = _json.get("source_image"); nullptr != val) {
    TypeDescLibrary::get<GUID>().visit(ComponentifyJSON(&_source_image, *val));
  }
}

void TexturePropsResource::_save() {
  _json.clear();
  TypeDescLibrary::get<TextureFilter>().visit(JSONifyComponent(_json, "filter", &_filter));
  TypeDescLibrary::get<BufferFormat>().visit(JSONifyComponent(_json, "format", &_format));
  TypeDescLibrary::get<Vec2i>().visit(JSONifyComponent(_json, "size", &_size));
  TypeDescLibrary::get<GUID>().visit(JSONifyComponent(_json, "guid", &_source_image));
  JSONResource::_save();
}

void TexturePropsResource::setFilter(TextureFilter filter) {
  _filter = filter;
  updated();
}

void TexturePropsResource::setFormat(BufferFormat format) {
  _format = format;
  updated();
}

void TexturePropsResource::setSize(const Vec2i& size) {
  _size = size;
  updated();
}

void TexturePropsResource::setSourceImage(const GUID& source_image) {
  _source_image = source_image;
  updated();
}

TextureFilter TexturePropsResource::getFilter() const {
  return _filter;
}

BufferFormat TexturePropsResource::getFormat() const {
  return _format;
}

const Vec2i& TexturePropsResource::getSize() const {
  return _size;
}

const GUID& TexturePropsResource::getSourceImage() const {
  return _source_image;
}

Resource::Type TexturePropsResource::type() const {
  return Type::TextureProps;
}