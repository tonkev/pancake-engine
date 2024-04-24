#include "resources/resource.hpp"

#include "resources/resources.hpp"
#include "util/fewi.hpp"

#include <filesystem>

#ifdef _WIN32
#include <windows.h>
//
#include <libloaderapi.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

using namespace pancake;
namespace fs = std::filesystem;

std::vector<std::string> Resource::_LOAD_PATHS{};

Resource::Resource(std::string_view path, const GUID& guid)
    : _path(path),
      _full_path(resolvePath(_path)),
      _dir(resolveDir(_full_path)),
      _ext(),
      _guid(guid),
      _gen(0) {
  standardisePath(_path);
  if (size_t ext_pos = path.find_last_of('.'); std::string::npos != ext_pos) {
    _ext = path.substr(ext_pos + 1, path.size());
  }
}

void Resource::load() {
  if (!_path.empty()) {
    _load();
    updated();
  }
}

void Resource::save() {
  if (!_path.empty()) {
    if (!(_dir.empty() || fs::exists(_dir))) {
      fs::create_directories(_dir);
    }
    _save();
  }
}

void Resource::ensureUpdated(Resources& resources) {}

void Resource::updated() {
  ++_gen;
  if ((std::numeric_limits<uint64_t>::max)() == _gen) {
    _gen = 0;
  }
}

std::string_view Resource::dir() const {
  return _dir;
}

std::string_view Resource::path() const {
  return _path;
}

std::string_view Resource::fullPath() const {
  return _full_path;
}

std::string_view Resource::extension() const {
  return _ext;
}

const pancake::GUID& Resource::guid() const {
  return _guid;
}

uint64_t Resource::gen() const {
  return _gen;
}

Resource& Resource::asResource() {
  return *this;
}

void Resource::standardisePath(std::string& path) {
  for (size_t i = 0; i < path.size(); ++i) {
    if (path[i] == '\\') {
      path[i] = '/';
    }
  }
}

void Resource::standardiseDirPath(std::string& path) {
  standardisePath(path);
  if (!path.ends_with('/')) {
    path.append("/");
  }
}

void Resource::setLoadPaths(const std::vector<std::string>& paths) {
  _LOAD_PATHS = paths;
  for (std::string& path : _LOAD_PATHS) {
    standardiseDirPath(path);
  }
}

std::string_view Resource::rootPath() {
  static const std::string root_path = []() -> std::string {
    const size_t max_path_size = 512;
    std::string root_path;
    root_path.resize(max_path_size);

#ifdef _WIN32
    GetModuleFileNameA(NULL, root_path.data(), max_path_size);
#endif

#ifdef __linux__
    readlink("/proc/self/exe", root_path.data(), max_path_size);
#endif

    size_t last_slash = 0;
    for (size_t i = 0; i < max_path_size - 1; ++i) {
      if (root_path[i] == '\0') {
        break;
      } else if ((root_path[i] == '\\') || (root_path[i] == '/')) {
        root_path[i] = '/';
        last_slash = i;
      }
    }
    root_path[last_slash + 1] = '\0';
    root_path.resize(last_slash + 1);

    return root_path;
  }();
  return root_path;
}

std::string Resource::resolvePath(std::string_view path) {
  std::string full_path{};

  std::string standard_path(path);
  standardisePath(standard_path);

  if (fs::exists(path)) {
    full_path = path;
  } else {
    for (const std::string& load_path : _LOAD_PATHS) {
      std::string p = load_path + standard_path;
      if (fs::exists(p)) {
        full_path = p;
        break;
      }
    }
  }

  if (full_path.empty()) {
    full_path = rootPath();
    full_path.append(standard_path);
  }
  return full_path;
}

std::string Resource::resolveDir(std::string_view path) {
  std::string dir;
  if (const std::size_t last_slash = path.find_last_of('/'); last_slash != std::string::npos) {
    dir = path.substr(0, last_slash);
  }
  return dir;
}