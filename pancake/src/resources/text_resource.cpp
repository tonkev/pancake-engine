#include "resources/text_resource.hpp"

#include "resources/resources.hpp"
#include "util/fewi.hpp"

#include <filesystem>
#include <fstream>

using namespace pancake;
namespace fs = std::filesystem;

Resources::Creator<TextResource> text_resource_creator;

const Resource::Type TextResource::TYPE = Type::Text;

TextResource::TextResource(std::string_view path, const GUID& guid)
    : Resource(path, guid), _text() {}

void TextResource::_load() {
  if (std::ifstream file(_full_path); file.is_open()) {
    _text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
  } else {
    FEWI::error("Failed to open file: " + _full_path + "!");
  }
}

void TextResource::_save() {
  if (std::ofstream file(_full_path, std::ios_base::trunc); file.is_open()) {
    file << _text;
    file.close();
  } else {
    FEWI::error("Failed to create/open file: " + _full_path + "!");
  }
}

std::string& TextResource::text() {
  updated();
  return _text;
}

std::string_view TextResource::constText() const {
  return _text;
}

Resource::Type TextResource::type() const {
  return Type::Text;
}