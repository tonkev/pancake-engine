#pragma once

#include "resources/resources.hpp"

#include <limits>
#include <tuple>

namespace pancake {
template <class T, ResourceableOrResourceInterfaceable Res, typename Tag>
struct ResourceUse {
 public:
  bool checkAndApplyResourceUpdates(T& self, Resources& resources) {
    const auto res_opt = resources.getOrCreate<Res>(guid);
    if (res_opt.has_value()) {
      Res& res = res_opt.value();

      if (uint64_t res_gen = res.asResource().gen(); res_gen != gen) {
        self.template resourceUpdated<Tag>(res);
        gen = res_gen;

        return true;
      }
    }

    return false;
  }

  GUID guid = GUID::null;
  uint64_t gen = std::numeric_limits<uint64_t>::max();
};

template <class T, class Super>
concept ResourceUseable = requires(T t) {
#ifndef __INTELLISENSE__
  []<ResourceableOrResourceInterfaceable Res, typename Tag>(ResourceUse<Super, Res, Tag>&) {}(t);
#else
  t;  // Disable this constraint for intellisense because it can't figure it out
#endif
};

template <class T, ResourceUseable<T>... Uses>
class ResourceUser {
 public:
  template <ResourceableOrResourceInterfaceable Res, typename Tag>
  void setResourceGuid(const GUID& guid) {
    auto& use = std::get<ResourceUse<T, Res, Tag>>(_uses);
    use.guid = guid;
    use.gen = std::numeric_limits<uint64_t>::max();
  }

  template <ResourceableOrResourceInterfaceable Res, typename Tag>
  const GUID& getResourceGuid() const {
    auto& use = std::get<ResourceUse<T, Res, Tag>>(_uses);
    return use.guid;
  }

  bool checkAndApplyResourceUpdates(Resources& resources) {
    T& instance = *static_cast<T*>(this);
    const bool results[] = {
        std::get<Uses>(_uses).checkAndApplyResourceUpdates(instance, resources)...};

    for (bool result : results) {
      if (result) {
        instance.resourcesUpdated();
        return true;
      }
    }

    return false;
  }

 private:
  std::tuple<Uses...> _uses;
};
}  // namespace pancake