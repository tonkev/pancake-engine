#pragma once

#include "pancake.hpp"
#include "resources/mesh_resource_interface.hpp"
#include "resources/resource.hpp"
#include "resources/resource_optional.hpp"
#include "resources/shader_resource_interface.hpp"
#include "util/guid.hpp"
#include "util/matrix.hpp"
#include "util/pstruct.hpp"

#include <algorithm>
#include <map>
#include <optional>
#include <string>

namespace pancake {
template <typename T>
concept ResourceInterfaceable =
    std::is_same_v<T, MeshResourceInterface> || std::is_same_v<T, ShaderResourceInterface>;

template <typename T>
concept ResourceableOrResourceInterfaceable = Resourceable<T> || ResourceInterfaceable<T>;

class Resources {
 public:
  class BaseCreator {
   public:
    BaseCreator(Resource::Type type);
    virtual ~BaseCreator() = default;

    virtual Resource* create(std::string_view path, const GUID& guid) const = 0;
    virtual MeshResourceInterface* castToMeshInterface(Resource* res) const = 0;
    virtual ShaderResourceInterface* castToShaderInterface(Resource* res) const = 0;
  };

  template <Resourceable T>
  class Creator : public BaseCreator {
   public:
    Creator() : BaseCreator(T::TYPE) {}
    virtual ~Creator() = default;

    virtual Resource* create(std::string_view path, const GUID& guid) const override {
      return new T(path, guid);
    }

    virtual MeshResourceInterface* castToMeshInterface(Resource* res) const override {
      if constexpr (std::is_base_of_v<MeshResourceInterface, T>) {
        return static_cast<MeshResourceInterface*>(static_cast<T*>(res));
      } else {
        return nullptr;
      }
    }

    virtual ShaderResourceInterface* castToShaderInterface(Resource* res) const override {
      if constexpr (std::is_base_of_v<ShaderResourceInterface, T>) {
        return static_cast<ShaderResourceInterface*>(static_cast<T*>(res));
      } else {
        return nullptr;
      }
    }
  };

  using ResourceMap = std::map<GUID, std::unique_ptr<Resource>>;

  Resources();
  ~Resources();

  template <Resourceable T>
  ResourceOptional<T> getOrCreate(const std::string& path, bool load = true) {
    ResourceOptional<T> res = std::nullopt;

    if (Resource* res_ptr = getOrCreate(path, load); (nullptr != res_ptr)) {
      if (T::TYPE == res_ptr->type()) {
        res = *static_cast<T*>(res_ptr);
      }
    } else {
      const Creators& creators = getCreators();
      if (const auto creator_it = creators.find(T::TYPE); (creator_it != creators.end())) {
        if (res_ptr = creator_it->second.get().create(path, GUID::gen()); (nullptr != res_ptr)) {
          if (load) {
            res_ptr->load();
          }
          res = *static_cast<T*>(res_ptr);
          _res_map.emplace(res_ptr->guid(), res_ptr);
          _import_map.emplace(res_ptr->path(), *res_ptr);
        }
      }
    }

    return res;
  }

  template <ResourceInterfaceable T>
  std::optional<std::reference_wrapper<T>> getOrCreate(const std::string& path, bool load = true) {
    std::optional<std::reference_wrapper<T>> interface = std::nullopt;

    if (Resource* res_ptr = getOrCreate(path, load); (nullptr != res_ptr)) {
      const Creators& creators = getCreators();
      if (const auto creator_it = creators.find(res_ptr->type()); (creator_it != creators.end())) {
        if constexpr (std::is_same_v<MeshResourceInterface, T>) {
          if (T* interface_ptr = creator_it->second.get().castToMeshInterface(res_ptr);
              (interface_ptr != nullptr)) {
            interface = *interface_ptr;
          }
        }
        if constexpr (std::is_same_v<ShaderResourceInterface, T>) {
          if (T* interface_ptr = creator_it->second.get().castToShaderInterface(res_ptr);
              (interface_ptr != nullptr)) {
            interface = *interface_ptr;
          }
        }
      }
    }

    return interface;
  }

  template <Resourceable T>
  ResourceOptional<T> getOrCreate(const GUID& guid) {
    ResourceOptional<T> res = std::nullopt;

    if (Resource* res_ptr = getOrCreate(guid);
        (nullptr != res_ptr) && (T::TYPE == res_ptr->type())) {
      res = *static_cast<T*>(res_ptr);
    }

    return res;
  }

  template <ResourceInterfaceable T>
  std::optional<std::reference_wrapper<T>> getOrCreate(const GUID& guid) {
    std::optional<std::reference_wrapper<T>> interface = std::nullopt;

    if (Resource* res_ptr = getOrCreate(guid); (nullptr != res_ptr)) {
      const Creators& creators = getCreators();
      if (const auto creator_it = creators.find(res_ptr->type()); (creator_it != creators.end())) {
        if constexpr (std::is_same_v<MeshResourceInterface, T>) {
          if (T* interface_ptr = creator_it->second.get().castToMeshInterface(res_ptr);
              (interface_ptr != nullptr)) {
            interface = *interface_ptr;
          }
        }
        if constexpr (std::is_same_v<ShaderResourceInterface, T>) {
          if (T* interface_ptr = creator_it->second.get().castToShaderInterface(res_ptr);
              (interface_ptr != nullptr)) {
            interface = *interface_ptr;
          }
        }
      }
    }

    return interface;
  }

  const ResourceMap& getResources() const;

 private:
  Resource* getOrCreate(const std::string& path, bool load = true);
  Resource* getOrCreate(const GUID& guid);

  PSTRUCT(ImportInfo)
  ImportInfo() = default;
  ImportInfo(const Resource& resource);
  PSTRUCT_MEMBER_INITIALISED(GUID, guid, GUID::null)
  PSTRUCT_MEMBER_INITIALISED(Resource::Type, type, Resource::Type::Text)
  PSTRUCT_END()

  struct SlashInvariantLess {
    bool operator()(const std::string& lhs, const std::string& rhs) const;
  };

  using ImportMap = std::map<std::string, ImportInfo, SlashInvariantLess>;

  ImportMap _import_map;
  ResourceMap _res_map;

  using Creators = std::map<Resource::Type, std::reference_wrapper<BaseCreator>>;
  static Creators& getCreators();

  friend BaseCreator;
};
}  // namespace pancake