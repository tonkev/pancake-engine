#pragma once

#include "util/assert.hpp"
#include "util/generational_index.hpp"

#include <iterator>

namespace pancake {
template <typename I>
struct ItemInfo {
  bool active;
  I gen;
};

template <typename T, typename I, int S>
class GenerationalArray {
 public:
  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::pair<GenerationalIndex<I>, T>;
    using pointer = std::pair<GenerationalIndex<I>, T*>;
    using reference = std::pair<GenerationalIndex<I>, T&>;

    Iterator(T* items, const ItemInfo<I>* infos, I id = 0) : _items(items), _infos(infos), _id(id) {
      while ((_id < static_cast<I>(S)) && (!_infos[_id].active)) {
        ++_id;
      }
    }

    reference operator*() { return {GenerationalIndex(_id, _infos[_id].gen), _items[_id]}; }

    pointer operator->() { return {GenerationalIndex(_id, _infos[_id].gen), &_items[_id]}; }

    Iterator& operator++() {
      ++_id;
      while ((_id < static_cast<I>(S)) && (!_infos[_id].active)) {
        ++_id;
      }
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& a, const Iterator& b) {
      return (a._items == b._items) && (a._infos == b._infos) && (a._id == b._id);
    };

    friend bool operator!=(const Iterator& a, const Iterator& b) {
      return (a._items != b._items) || (a._infos != b._infos) || (a._id != b._id);
    };

   private:
    T* _items;
    const ItemInfo<I>* _infos;
    I _id;
  };

  struct ConstIterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::pair<GenerationalIndex<I>, T>;
    using pointer = std::pair<GenerationalIndex<I>, const T*>;
    using reference = std::pair<GenerationalIndex<I>, const T&>;

    ConstIterator(const T* items, const ItemInfo<I>* infos, I id = 0)
        : _items(items), _infos(infos), _id(id) {
      while ((_id < static_cast<I>(S)) && (!_infos[_id].active)) {
        ++_id;
      }
    }

    reference operator*() { return {GenerationalIndex(_id, _infos[_id].gen), _items[_id]}; }

    pointer operator->() { return {GenerationalIndex(_id, _infos[_id].gen), &_items[_id]}; }

    ConstIterator& operator++() {
      ++_id;
      while ((_id < static_cast<I>(S)) && (!_infos[_id].active)) {
        ++_id;
      }
      return *this;
    }

    ConstIterator operator++(int) {
      ConstIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const ConstIterator& a, const ConstIterator& b) {
      return (a._items == b._items) && (a._infos == b._infos) && (a._id == b._id);
    };

    friend bool operator!=(const ConstIterator& a, const ConstIterator& b) {
      return (a._items != b._items) || (a._infos != b._infos) || (a._id != b._id);
    };

   private:
    const T* _items;
    const ItemInfo<I>* _infos;
    I _id;
  };

  GenerationalArray() : _nextId(0) {
    for (ItemInfo<I>& info : _infos) {
      info.active = false;
      info.gen = 0;
    }
  }
  ~GenerationalArray() {}

  bool has(const GenerationalIndex<I>& idx) const {
    if ((GenerationalIndex<I>::null != idx) && (0 <= idx.id) && (idx.id < static_cast<I>(S))) {
      const ItemInfo<I>& info = _infos[idx.id];
      return info.active && (idx.gen == _infos[idx.id].gen);
    }
    return false;
  }

  GenerationalIndex<I> insert(const T& item = T()) {
    GenerationalIndex<I> index = GenerationalIndex<I>::null;
    const I firstId = _nextId;
    do {
      ItemInfo<I>& info = _infos[_nextId];
      if (!info.active) {
        _items[_nextId] = item;
        index.id = _nextId;
        index.gen = info.gen;
        info.active = true;
        break;
      }
      if (static_cast<I>(S) <= ++_nextId) {
        _nextId = 0;
      }
    } while (firstId != _nextId);
    if (static_cast<I>(S) <= ++_nextId) {
      _nextId = 0;
    }
    return index;
  }

  void insert(const GenerationalIndex<I>& idx, const T& item = T()) {
    ensure((0 <= idx.id) && (idx.id < static_cast<I>(S)) && (!_infos[idx.id].active));
    ItemInfo<I>& info = _infos[idx.id];
    _items[idx.id] = item;
    info.active = true;
    info.gen = idx.gen;
  }

  void set(const GenerationalIndex<I>& idx, const T& item = T()) {
    if (has(idx)) {
      ItemInfo<I>& info = _infos[idx.id];
      if (info.active && (info.gen == idx.gen)) {
        info.active = true;
        info.gen = idx.gen;
        _items[idx.id] = item;
      }
    }
  }

  void remove(const GenerationalIndex<I>& idx) {
    if (has(idx)) {
      ItemInfo<I>& info = _infos[idx.id];
      if (info.active && (info.gen == idx.gen)) {
        info.active = false;
        ++info.gen;
      }
    }
  }

  void clear() {
    for (ItemInfo<I>& info : _infos) {
      info.active = false;
      info.gen = 0;
    }
    _nextId = 0;
  }

  size_t size() const {
    size_t count = 0;
    for (I i = 0; i < static_cast<I>(S); ++i) {
      if (_infos[i].active) {
        ++count;
      }
    }
    return count;
  }

  Iterator begin() { return Iterator(_items, _infos); }
  Iterator end() { return Iterator(_items, _infos, static_cast<I>(S)); }

  ConstIterator begin() const { return ConstIterator(_items, _infos); }
  ConstIterator end() const { return ConstIterator(_items, _infos, static_cast<I>(S)); }

  T& operator[](const GenerationalIndex<I>& idx) {
    ensure(has(idx));
    return _items[idx.id];
  }
  const T& operator[](const GenerationalIndex<I>& idx) const {
    ensure(has(idx));
    return _items[idx.id];
  }

 private:
  T _items[S];
  ItemInfo<I> _infos[S];
  I _nextId;
};
}  // namespace pancake