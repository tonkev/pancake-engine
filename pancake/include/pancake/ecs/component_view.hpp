#pragma once

#include "ecs/common.hpp"
#include "util/type_desc_library.hpp"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace pancake {
class World;
class ComponentView {
 public:
  using ComponentCache = std::unordered_map<std::reference_wrapper<const TypeDesc>,
                                            std::vector<void*>,
                                            std::hash<TypeDesc>,
                                            std::equal_to<TypeDesc>>;

  template <typename... Ts>
  class Formatter {
   public:
    struct Iterator {
     public:
      using iterator_category = std::forward_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = std::tuple<Ts*...>;
      using pointer = const std::tuple<Ts*...>*;
      using reference = const std::tuple<Ts*...>&;

      Iterator(unsigned int idx, const ComponentCache& cache)
          : _idx(idx), _cache(cache), _view({static_cast<Ts*>(nullptr)...}) {
        fill();
      }

      reference operator*() { return _view; }

      pointer operator->() { return _view; }

      Iterator& operator++() {
        ++_idx;
        fill();
        return *this;
      }

      Iterator operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
      }

      friend bool operator==(const Iterator& a, const Iterator& b) {
        return (a._idx == b._idx) && (&a._cache == &b._cache);
      };

      friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); };

     private:
      void fill() {
        if ((!_cache.empty()) && (_idx < _cache.begin()->second.size())) {
          _view = {reinterpret_cast<Ts*>(_cache.at(TypeDescLibrary::get<Ts>())[_idx])...};
        }
      }

      unsigned int _idx;
      const ComponentCache& _cache;
      std::tuple<Ts*...> _view;
    };

    Formatter(const ComponentCache& cache) : _cache(cache) {}

    Iterator begin() const { return Iterator(0, _cache); }
    Iterator end() const {
      return Iterator(_cache.empty() ? 0 : static_cast<unsigned int>(_cache.begin()->second.size()),
                      _cache);
    }

   private:
    const ComponentCache& _cache;
  };

  ComponentView(const ComponentMask& mask, World& world);
  ~ComponentView() = default;

  void invalidate();
  void update();

  template <typename... Ts>
  Formatter<Ts...> get() {
    update();
    return Formatter<Ts...>(_cache);
  }

 private:
  const ComponentMask _mask;
  World& _world;
  ComponentCache _cache;
  bool _valid_cache;
  std::mutex _update_mutex;
};
}  // namespace pancake