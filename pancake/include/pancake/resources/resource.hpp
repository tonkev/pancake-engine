#pragma once

#include "util/guid.hpp"

#include <concepts>
#include <set>
#include <string>
#include <vector>

namespace pancake {
class Resources;
class Resource {
 public:
  enum class Type {
    Text,
    Image,
    Json,
    Tileset,
    TextureProps,
    LDtk,
    QuakeMap,
    Obj,
    ObjMesh,
    Gl3Shader,
    Material,
    Gltf,
    GltfMesh
  };

  virtual ~Resource() = default;

  void load();
  void save();

  virtual void ensureUpdated(Resources& resources);

  std::string_view dir() const;
  std::string_view path() const;
  std::string_view fullPath() const;
  std::string_view extension() const;

  const GUID& guid() const;
  uint64_t gen() const;

  Resource& asResource();

  virtual Type type() const = 0;

  static void standardisePath(std::string& path);
  static void standardiseDirPath(std::string& path);

  static void setLoadPaths(const std::vector<std::string>& paths);

  static std::string_view rootPath();
  static std::string resolvePath(std::string_view path);
  static std::string resolveDir(std::string_view path);

 protected:
  Resource(std::string_view path, const GUID& guid);

  void updated();

  virtual void _load() = 0;
  virtual void _save() = 0;

  template <typename T>
  friend class ResourceAccessor;

  std::string _path;
  std::string _full_path;
  std::string _dir;
  std::string _ext;

 private:
  GUID _guid;
  uint64_t _gen;

  static std::vector<std::string> _LOAD_PATHS;
};

template <typename T>
concept Resourceable = std::is_base_of_v<Resource, T>;
}  // namespace pancake