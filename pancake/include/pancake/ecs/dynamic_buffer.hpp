#pragma once

#include <concepts>
#include <cstring>
#include <string>

namespace pancake {
template <typename T, int N>
class DynamicBuffer {
 public:
  using element_type = T;

  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    Iterator(T* pos, T* end) : _pos(pos), _end(end) {}

    reference operator*() { return *_pos; }

    pointer operator->() { return _pos; }

    Iterator& operator++() {
      if (_pos != _end) {
        ++_pos;
      }
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& a, const Iterator& b) {
      return (a._pos == b._pos) && (a._end == b._end);
    };

    friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); };

   private:
    T* _pos;
    T* _end;
  };

  struct ConstIterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;

    ConstIterator(const T* pos, const T* end) : _pos(pos), _end(end) {}

    reference operator*() { return *_pos; }

    pointer operator->() { return _pos; }

    ConstIterator& operator++() {
      if (_pos != _end) {
        ++_pos;
      }
      return *this;
    }

    ConstIterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const ConstIterator& a, const ConstIterator& b) {
      return (a._pos == b._pos) && (a._end == b._end);
    };

    friend bool operator!=(const ConstIterator& a, const ConstIterator& b) { return !(a == b); };

   private:
    const T* _pos;
    const T* _end;
  };

  DynamicBuffer() : _size(0), _heap_capacity(0), _local(), _heap(nullptr) {}

  ~DynamicBuffer() {
    if (0 < _heap_capacity) {
      delete[] _heap;
    }
  }

  T& emplaceBack() {
    ensureCapacity(_size + 1);
    T* buffer = data();
    T& new_element = buffer[_size++];
    new_element = T();
    return new_element;
  }

  void ensureCapacity(size_t new_capacity) {
    if (0 < _heap_capacity) {
      if (_heap_capacity < new_capacity) {
        T* new_heap = new T[new_capacity];
        std::memcpy(new_heap, _heap, _size);
        delete[] _heap;
        _heap = new_heap;
        _heap_capacity = new_capacity;
      }
    } else if (N < new_capacity) {
      T* new_heap = new T[new_capacity];
      std::memcpy(new_heap, _local, _size);
      _heap = new_heap;
      _heap_capacity = new_capacity;
    }
  }

  void resize(size_t new_size) {
    ensureCapacity(new_size);
    _size = new_size;
  }

  Iterator begin() {
    if (0 < _heap_capacity) {
      return Iterator(_heap, _heap + _size);
    }
    return Iterator(_local, _local + _size);
  }

  ConstIterator begin() const {
    if (0 < _heap_capacity) {
      return ConstIterator(_heap, _heap + _size);
    }
    return ConstIterator(_local, _local + _size);
  }

  Iterator end() {
    if (0 < _heap_capacity) {
      return Iterator(_heap + _size, _heap + _size);
    }
    return Iterator(_local + _size, _local + _size);
  }

  ConstIterator end() const {
    if (0 < _heap_capacity) {
      return ConstIterator(_heap + _size, _heap + _size);
    }
    return ConstIterator(_local + _size, _local + _size);
  }

  size_t size() const { return _size; }

  T* data() { return (0 < _heap_capacity) ? _heap : _local; }
  const T* data() const { return (0 < _heap_capacity) ? _heap : _local; }

  void clear() { _size = 0; }

  void destroy() {
    if (0 < _heap_capacity) {
      delete[] _heap;
    }
    _heap_capacity = 0;
    _size = 0;
  }

  T& operator[](int index) {
    if (0 < _heap_capacity) {
      return _heap[index];
    }
    return _local[index];
  }

  const T& operator[](int index) const {
    if (0 < _heap_capacity) {
      return _heap[index];
    }
    return _local[index];
  }

 private:
  size_t _size;
  size_t _heap_capacity;
  T* _heap;
  T _local[N];
};

template <typename>
struct IsDynamicBuffer : std::false_type {};

template <typename T, size_t N>
struct IsDynamicBuffer<DynamicBuffer<T, N>> : std::true_type {};

template <typename T>
concept DynamicBufferable = IsDynamicBuffer<T>::value;
}  // namespace pancake