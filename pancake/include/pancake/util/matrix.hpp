#pragma once

#include "util/type_desc_library.hpp"

#include <cmath>
#include <cstring>
#include <iostream>
#include <numbers>
#include <type_traits>

namespace pancake {
template <typename T, int W, int H>
class Matrix {
 public:
  Matrix(T fill = 0) {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        m[x][y] = fill;
      }
    }
  }

  Matrix(T x, T y) : Matrix(0) {
    if constexpr (0 < W) {
      if constexpr (0 < H) {
        m[0][0] = x;
      }
      if constexpr (1 < H) {
        m[0][1] = y;
      }
    }
  }

  Matrix(T x, T y, T z) : Matrix(0) {
    if constexpr (0 < W) {
      if constexpr (0 < H) {
        m[0][0] = x;
      }
      if constexpr (1 < H) {
        m[0][1] = y;
      }
      if constexpr (2 < H) {
        m[0][2] = z;
      }
    }
  }

  Matrix(const Matrix<T, 1, 2>& xy, T z) : Matrix(0) {
    if constexpr (0 < W) {
      if constexpr (0 < H) {
        m[0][0] = xy.m[0][0];
      }
      if constexpr (1 < H) {
        m[0][1] = xy.m[0][1];
      }
      if constexpr (2 < H) {
        m[0][2] = z;
      }
    }
  }

  Matrix(const Matrix<T, 1, 3>& xyz, T w) : Matrix(0) {
    if constexpr (0 < W) {
      if constexpr (0 < H) {
        m[0][0] = xyz.m[0][0];
      }
      if constexpr (1 < H) {
        m[0][1] = xyz.m[0][1];
      }
      if constexpr (2 < H) {
        m[0][2] = xyz.m[0][2];
      }
      if constexpr (3 < H) {
        m[0][3] = w;
      }
    }
  }

  Matrix(T x, T y, T z, T w) : Matrix(0) {
    if constexpr (0 < W) {
      if constexpr (0 < H) {
        m[0][0] = x;
      }
      if constexpr (1 < H) {
        m[0][1] = y;
      }
      if constexpr (2 < H) {
        m[0][2] = z;
      }
      if constexpr (3 < H) {
        m[0][3] = w;
      }
    }
  }

  Matrix(const Matrix<T, W, H>& other) { std::memcpy(m, other.m, sizeof(T[W][H])); }

  Matrix(std::initializer_list<T> l) {
    for (unsigned int i = 0; i < l.size() && i < W * H; ++i) {
      m[i % W][i / W] = *(l.begin() + i);
    }
  }

  ~Matrix() {}

  template <typename Q = T>
  static typename std::enable_if<W == H, const Matrix<Q, W, H>&>::type identity() {
    static const Matrix<Q, W, H> m = [] {
      Matrix<Q, W, H> m;
      for (int i = 0; i < W; ++i) {
        m.m[i][i] = ((Q)1);
      };
      return m;
    }();
    return m;
  }

  static Matrix<T, W, H> zeros() { return Matrix<T, W, H>(0); }

  static Matrix<T, W, H> ones() { return Matrix<T, W, H>(1); }

  template <typename Q = T>
  static typename std::enable_if<(W == H) && (W == 3), Matrix<Q, W, H>>::type translation(T x,
                                                                                          T y) {
    Matrix<T, 3, 3> matrix = identity();
    matrix[2][0] = x;
    matrix[2][1] = y;
    return matrix;
  }

  template <typename Q = T, int S>
  static typename std::enable_if<(W == H) && (W == 3) && (S == 2), Matrix<Q, W, H>>::type
  translation(const Matrix<T, 1, S>& vec) {
    return translation(vec[0][0], vec[0][1]);
  }

  template <typename Q = T>
  static typename std::enable_if<(W == H) && (W == 4), Matrix<Q, W, H>>::type translation(T x,
                                                                                          T y,
                                                                                          T z) {
    Matrix<T, 4, 4> matrix = identity();
    matrix[3][0] = x;
    matrix[3][1] = y;
    matrix[3][2] = z;
    return matrix;
  }

  template <typename Q = T, int S>
  static
      typename std::enable_if<(W == H) && (W == 4) && ((S == 3) || (S == 4)), Matrix<Q, W, H>>::type
      translation(const Matrix<T, 1, S>& vec) {
    return translation(vec[0][0], vec[0][1], vec[0][2]);
  }

  template <typename Q = T>
  static typename std::enable_if<(W == H) && (W == 3), Matrix<Q, W, H>>::type rotation(T r) {
    Matrix<T, 3, 3> matrix = identity();

    matrix[0][0] = std::cos(r);
    matrix[0][1] = std::sin(r);
    matrix[1][0] = -matrix[0][1];
    matrix[1][1] = matrix[0][0];

    return matrix;
  }

  template <typename Q = T>
  static typename std::enable_if<(W == H) && (W == 4), Matrix<Q, W, H>>::type rotation(T x,
                                                                                       T y,
                                                                                       T z) {
    Matrix<T, 4, 4> matrix = identity();

    matrix[1][1] = std::cos(x);
    matrix[2][1] = -std::sin(x);
    matrix[1][2] = -matrix[2][1];
    matrix[2][2] = matrix[1][1];

    matrix[0][0] = std::cos(y);
    matrix[2][0] = std::sin(y);
    matrix[0][2] = -matrix[2][0];
    matrix[2][2] = matrix[0][0];

    matrix[0][0] = std::cos(z);
    matrix[0][1] = std::sin(z);
    matrix[1][0] = -matrix[0][1];
    matrix[1][1] = matrix[0][0];

    return matrix;
  }

  template <typename Q = T, int S>
  static
      typename std::enable_if<(W == H) && (W == 4) && ((S == 3) || (S == 4)), Matrix<Q, W, H>>::type
      rotation(const Matrix<T, 1, S>& vec) {
    return rotation(vec[0][0], vec[0][1], vec[0][2]);
  }

  template <typename Q = T>
  static typename std::enable_if<(W == H) && (W == 3), Matrix<Q, W, H>>::type scale(T x, T y) {
    Matrix<T, 3, 3> matrix = identity();
    matrix[0][0] = x;
    matrix[1][1] = y;
    return matrix;
  }

  template <typename Q = T, int S>
  static typename std::enable_if<(W == H) && (W == 3) && (S == 2), Matrix<Q, W, H>>::type scale(
      const Matrix<T, 1, S>& vec) {
    return scale(vec[0][0], vec[0][1]);
  }

  template <typename Q = T>
  static typename std::enable_if<(W == H) && (W == 4), Matrix<Q, W, H>>::type scale(T x, T y, T z) {
    Matrix<T, 4, 4> matrix = identity();
    matrix[0][0] = x;
    matrix[1][1] = y;
    matrix[2][2] = z;
    return matrix;
  }

  template <typename Q = T, int S>
  static
      typename std::enable_if<(W == H) && (W == 4) && ((S == 3) || (S == 4)), Matrix<Q, W, H>>::type
      scale(const Matrix<T, 1, S>& vec) {
    return scale(vec[0][0], vec[0][1], vec[0][2]);
  }

  template <typename Q = T, int S>
  static typename std::enable_if<(W == H) && (W == 3) && (S == 2), Matrix<Q, W, H>>::type
  transform(const Matrix<T, 1, S>& t, T r, const Matrix<T, 1, S>& s) {
    return translation(t) * (rotation(r) * scale(s));
  }

  template <typename Q = T, int S>
  static
      typename std::enable_if<(W == H) && (W == 4) && ((S == 3) || (S == 4)), Matrix<Q, W, H>>::type
      transform(const Matrix<T, 1, S>& t, const Matrix<T, 1, S>& r, const Matrix<T, 1, S>& s) {
    return translation(t) * (rotation(r) * scale(s));
  }

  static Matrix<T, 4, 4> ortographic(T width, T height, T depth) {
    return Matrix<T, 4, 4>(
        {2.f / width, 0, 0, 0, 0, 2.f / height, 0, 0, 0, 0, 2.f / depth, -1, 0, 0, 0, 1});
  }

  static Matrix<T, 4, 4> perspective(T fov, T aspect, T near, T far) {
    float s = 1.0f / std::tan(fov * 0.5f * static_cast<T>(std::numbers::pi) / 180.f);
    return Matrix<T, 4, 4>({s, 0, 0, 0, /**/ 0, s / aspect, 0, 0, /**/ 0, 0,
                            (far + near) / (near - far), (2 * far * near) / (near - far), /**/ 0, 0,
                            -1, 0});
  }

  T* operator[](int i) { return m[i]; }
  const T* operator[](int i) const { return m[i]; }

  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 1, Q>::type& x() {
    return m[0][0];
  }
  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 2, Q>::type& y() {
    return m[0][1];
  }
  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 3, Q>::type& z() {
    return m[0][2];
  }
  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 4, Q>::type& w() {
    return m[0][3];
  }

  template <typename Q = T>
  typename std::enable_if<(W == 1) && (H >= 2), Matrix<Q, 1, 2>&>::type xy() {
    return reinterpret_cast<Matrix<Q, 1, 2>&>(*this);
  }

  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 1, const Q>::type& x() const {
    return m[0][0];
  }
  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 2, const Q>::type& y() const {
    return m[0][1];
  }
  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 3, const Q>::type& z() const {
    return m[0][2];
  }
  template <typename Q = T>
  constexpr typename std::enable_if<W == 1 && H >= 4, const Q>::type& w() const {
    return m[0][3];
  }

  template <typename Q = T>
  typename std::enable_if<(W == 1) && (H >= 2), const Matrix<Q, 1, 2>&>::type xy() const {
    return reinterpret_cast<const Matrix<Q, 1, 2>&>(*this);
  }

  template <typename Q = T>
  typename std::enable_if<(W == 1) && (H >= 3), Matrix<Q, 1, 3>>::type xyz() const {
    return Matrix<Q, 1, 3>(m[0][0], m[0][1], m[0][2]);
  }

  template <typename Q = T>
  typename std::enable_if<(W == H) && (W == 4), Matrix<Q, 1, 3>>::type forward() const {
    return ((*this) * Matrix<T, 1, 4>(0, 0, 1, 0)).xyz();
  }

  template <typename Q = T>
  typename std::enable_if<(W == H) && (W == 4), Matrix<Q, 1, 3>>::type up() const {
    return ((*this) * Matrix<T, 1, 4>(0, 1, 0, 0)).xyz();
  }

  template <typename Q = T>
  typename std::enable_if<(W == H) && (W == 4), Matrix<Q, 1, 3>>::type right() const {
    return ((*this) * Matrix<T, 1, 4>(1, 0, 0, 0)).xyz();
  }

  Matrix<T, W, H> abs() const {
    Matrix<T, W, H> abs;
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        T v = m[x][y];
        abs[x][y] = std::abs(v);
      }
    }
    return abs;
  }

  Matrix<T, W, H> sign() const {
    Matrix<T, W, H> sign = abs();
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        if (sign[x][y] < 0.000001f) {
          sign[x][y] = 0.f;
        } else {
          sign[x][y] = m[x][y] / sign[x][y];
        }
      }
    }
    return sign;
  }

  Matrix<T, H, W> transpose() const {
    Matrix<T, H, W> transpose;
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        transpose[y][x] = m[x][y];
      }
    }
    return transpose;
  }

  Matrix<T, W, H> mask(const Matrix<T, W, H>& other) const {
    Matrix<T, W, H> result;
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        result[x][y] = m[x][y] * other[x][y];
      }
    }
    return result;
  }

  T squaredNorm() const {
    T total = (T)0;
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        total += (m[x][y] * m[x][y]);
      }
    }
    return total;
  }

  T norm() const { return static_cast<T>(std::sqrt(squaredNorm())); }

  Matrix<T, W, H> normalised() const { return (*this) * (1 / norm()); }

  Matrix<T, W, H> reciprocal() const {
    Matrix<T, W, H> result;
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        result[x][y] = 1 / m[x][y];
      }
    }
    return result;
  }

  template <typename Q = T>
  typename std::enable_if<W == 1 && H == 2, Matrix<Q, W, H>>::type perpendicular() const {
    return Matrix<T, W, H>(-y(), x());
  }

  template <typename Q = T>
  typename std::enable_if<W == 1 && H == 2, Q>::type atan2() const {
    return std::atan2(y(), x());
  }

  template <typename Q = T>
  typename std::enable_if<W == 1, Q>::type angle(Matrix<T, 1, H> other) const {
    return std::acos(dot(other) / (norm() * other.norm()));
  }

  template <typename Q = T>
  typename std::enable_if<W == 1, Q>::type dot(const Matrix<T, 1, H>& rhs) const {
    return ((*this).transpose() * rhs).x();
  }

  template <typename Q = T>
  typename std::enable_if<W == 1 && H == 3, Matrix<Q, 1, 3>>::type cross(
      const Matrix<T, 1, 3>& rhs) const {
    return Matrix<T, 1, 3>((y() * rhs.z()) - (z() * rhs.y()), (z() * rhs.x()) - (x() * rhs.z()),
                           (x() * rhs.y()) - (y() * rhs.x()));
  }

  template <typename Q = T>
  typename std::enable_if<W == H && W == 2, Q>::type determinant() const {
    return (m[0][0] * m[1][1]) - (m[1][0] * m[0][1]);
  }
  template <typename Q = T, typename R = T>
  typename std::enable_if<W == H && W == 3, Q>::type determinant() const {
    return (m[0][0] * ((m[1][1] * m[2][2]) - (m[2][1] * m[1][2]))) -
           (m[0][1] * ((m[1][0] * m[2][2]) - (m[1][2] * m[2][0]))) +
           (m[0][2] * ((m[1][0] * m[2][1]) - (m[1][1] * m[2][0])));
  }
  template <typename Q = T, typename R = T, typename S = T>
  typename std::enable_if<W == H && W == 4, Q>::type determinant() const {
    return (m[0][0] * ((m[1][1] * ((m[2][2] * m[3][3]) - (m[3][2] * m[2][3]))) -
                       (m[1][2] * ((m[2][1] * m[3][3]) - (m[2][3] * m[3][1]))) +
                       (m[1][3] * ((m[2][1] * m[3][2]) - (m[2][2] * m[3][1]))))) -
           (m[1][0] * ((m[0][1] * ((m[2][2] * m[3][3]) - (m[3][2] * m[2][3]))) -
                       (m[0][2] * ((m[2][1] * m[3][3]) - (m[2][3] * m[3][1]))) +
                       (m[0][3] * ((m[2][1] * m[3][2]) - (m[2][2] * m[3][1]))))) +
           (m[2][0] * ((m[0][1] * ((m[1][2] * m[3][3]) - (m[3][2] * m[1][3]))) -
                       (m[0][2] * ((m[1][1] * m[3][3]) - (m[1][3] * m[3][1]))) +
                       (m[0][3] * ((m[1][1] * m[3][2]) - (m[1][2] * m[3][1]))))) -
           (m[3][0] * ((m[0][1] * ((m[1][2] * m[2][3]) - (m[2][2] * m[1][3]))) -
                       (m[0][2] * ((m[1][1] * m[2][3]) - (m[1][3] * m[2][1]))) +
                       (m[0][3] * ((m[1][1] * m[2][2]) - (m[1][2] * m[2][1])))));
  }

  template <typename Q = T>
  typename std::enable_if<W == H && W == 3, Matrix<Q, W, H>>::type inverse() const {
    Matrix<T, W, H> matrix;
    T det = determinant();

    if (det != 0) {
      T invdet = ((Q)1) / det;

      matrix[0][0] = m[1][1] * m[2][2] - m[2][1] * m[1][2];
      matrix[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
      matrix[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
      matrix[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
      matrix[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
      matrix[1][2] = m[1][0] * m[0][2] - m[0][0] * m[1][2];
      matrix[2][0] = m[1][0] * m[2][1] - m[2][0] * m[1][1];
      matrix[2][1] = m[2][0] * m[0][1] - m[0][0] * m[2][1];
      matrix[2][2] = m[0][0] * m[1][1] - m[1][0] * m[0][1];

      matrix = matrix * invdet;
    }

    return matrix;
  }

  template <typename Q = T>
  typename std::enable_if<W == H && W == 4, Matrix<Q, 1, 3>>::type getTranslation() const {
    return Matrix<Q, 1, 3>({m[3][0], m[3][1], m[3][2]});
  }

  template <typename Q = T>
  typename std::enable_if<W == H && W == 4, Matrix<Q, 4, 4>>::type getRotation() const {
    Matrix<Q, 4, 4> rotation(*this);
    rotation[3][0] = (T)0;
    rotation[3][1] = (T)0;
    rotation[3][2] = (T)0;
    Matrix<Q, 1, 3> inv_scale = getScale().reciprocal();
    rotation[0][0] *= inv_scale.x();
    rotation[0][1] *= inv_scale.x();
    rotation[0][2] *= inv_scale.x();
    rotation[1][0] *= inv_scale.y();
    rotation[1][1] *= inv_scale.y();
    rotation[1][2] *= inv_scale.y();
    rotation[2][0] *= inv_scale.z();
    rotation[2][1] *= inv_scale.z();
    rotation[2][2] *= inv_scale.z();
    return rotation;
  }

  template <typename Q = T>
  typename std::enable_if<W == H && W == 3, Matrix<Q, 1, 2>>::type getScale() const {
    return Matrix<Q, 1, 2>(Matrix<T, 1, 2>({m[0][0], m[0][1]}).norm(),
                           Matrix<T, 1, 2>({m[1][0], m[1][1]}).norm());
  }

  template <typename Q = T>
  typename std::enable_if<W == H && W == 4, Matrix<Q, 1, 3>>::type getScale() const {
    return Matrix<Q, 1, 3>({Matrix<T, 1, 3>({m[0][0], m[0][1], m[0][2]}).norm(),
                            Matrix<T, 1, 3>({m[1][0], m[1][1], m[1][2]}).norm(),
                            Matrix<T, 1, 3>({m[2][0], m[2][1], m[2][2]}).norm()});
  }

  Matrix<T, W, H> min(const Matrix<T, W, H>& other) const {
    Matrix<T, W, H> minimum;
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        minimum[x][y] = std::min(m[x][y], other.m[x][y]);
      }
    }
    return minimum;
  }

  Matrix<T, W, H> max(const Matrix<T, W, H>& other) const {
    Matrix<T, W, H> maximum;
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        maximum[x][y] = std::max(m[x][y], other.m[x][y]);
      }
    }
    return maximum;
  }

  bool within(const Matrix<T, W, H>& minimum, const Matrix<T, W, H>& maximum) const {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        if ((maximum.m[x][y] < m[x][y]) || (m[x][y] < minimum.m[x][y])) {
          return false;
        }
      }
    }
    return true;
  }

  bool approximatelyEqual(const Matrix<T, W, H>& other, T margin) const {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        T a = m[x][y];
        T b = other.m[x][y];
        if (((a < b) && (margin < (b - a))) || ((b < a) && (margin < (a - b)))) {
          return false;
        }
      }
    }
    return true;
  }

  template <typename Q>
  operator Matrix<Q, W, H>() const {
    Matrix<Q, W, H> other;
    for (unsigned int i = 0; i < W; ++i) {
      for (unsigned int j = 0; j < H; ++j) {
        other.m[i][j] = static_cast<Q>(m[i][j]);
      }
    }
    return other;
  }

  T m[W][H];

 private:
  static inline const TypeDesc& DESC
#ifdef __linux__
      __attribute__((used))
#endif
      = TypeDescLibrary::get<Matrix<T, W, H>>().addField("m", 0, TypeDescLibrary::get<T[W][H]>());
};

using Mat3f = Matrix<float, 3, 3>;
using Mat4f = Matrix<float, 4, 4>;

using Vec2i = Matrix<int, 1, 2>;
using Vec3i = Matrix<int, 1, 3>;
using Vec4i = Matrix<int, 1, 4>;

using Vec2u = Matrix<unsigned int, 1, 2>;
using Vec3u = Matrix<unsigned int, 1, 3>;
using Vec4u = Matrix<unsigned int, 1, 4>;

using Vec2f = Matrix<float, 1, 2>;
using Vec3f = Matrix<float, 1, 3>;
using Vec4f = Matrix<float, 1, 4>;

template <typename T, int W, int H>
Matrix<T, W, H> operator*(const Matrix<T, W, H>& matrix, const T scalar) {
  Matrix<T, W, H> result;
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = matrix[x][y] * scalar;
    }
  }
  return result;
}
template <typename T, int W, int H>
Matrix<T, W, H> operator*(const T scalar, const Matrix<T, W, H>& matrix) {
  return matrix * scalar;
}

template <typename T, int W, int H, int I>
Matrix<T, I, H> operator*(const Matrix<T, W, H>& matrixA, const Matrix<T, I, W>& matrixB) {
  Matrix<T, I, H> result;
  for (int x = 0; x < I; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = 0;
      for (int z = 0; z < W; ++z) {
        result[x][y] += matrixA[z][y] * matrixB[x][z];
      }
    }
  }
  return result;
}

template <typename T, int W, int H>
Matrix<T, W, H> operator/(const Matrix<T, W, H>& matrix, const T scalar) {
  Matrix<T, W, H> result;
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = matrix[x][y] / scalar;
    }
  }
  return result;
}

template <typename T, int W, int H>
Matrix<T, W, H> operator-(const Matrix<T, W, H>& matrix, const T scalar) {
  Matrix<T, W, H> result;
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = matrix[x][y] - scalar;
    }
  }
  return result;
}
template <typename T, int W, int H>
Matrix<T, W, H> operator-(const T scalar, const Matrix<T, W, H>& matrix) {
  return matrix - scalar;
}

template <typename T, int W, int H>
Matrix<T, W, H> operator-(const Matrix<T, W, H>& matrixA, const Matrix<T, W, H>& matrixB) {
  Matrix<T, W, H> result;
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = matrixA[x][y] - matrixB[x][y];
    }
  }
  return result;
}

template <typename T, int W, int H>
Matrix<T, W, H>& operator-=(Matrix<T, W, H>& matrix, const T scalar) {
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      matrix[x][y] -= scalar;
    }
  }
  return matrix;
}

template <typename T, int W, int H>
Matrix<T, W, H>& operator-=(Matrix<T, W, H>& matrixA, const Matrix<T, W, H>& matrixB) {
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      matrixA[x][y] -= matrixB[x][y];
    }
  }
  return matrixA;
}

template <typename T, int W, int H>
Matrix<T, W, H> operator-(const Matrix<T, W, H>& matrix) {
  Matrix<T, W, H> result;
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = -matrix[x][y];
    }
  }
  return result;
}

template <typename T, int W, int H>
Matrix<T, W, H> operator+(const Matrix<T, W, H>& matrix, const T scalar) {
  Matrix<T, W, H> result;
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = matrix[x][y] + scalar;
    }
  }
  return result;
}
template <typename T, int W, int H>
Matrix<T, W, H> operator+(const T scalar, const Matrix<T, W, H>& matrix) {
  return matrix + scalar;
}

template <typename T, int W, int H>
Matrix<T, W, H> operator+(const Matrix<T, W, H>& matrixA, const Matrix<T, W, H>& matrixB) {
  Matrix<T, W, H> result;
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      result[x][y] = matrixA[x][y] + matrixB[x][y];
    }
  }
  return result;
}

template <typename T, int W, int H>
Matrix<T, W, H>& operator+=(Matrix<T, W, H>& matrix, const T scalar) {
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      matrix[x][y] += scalar;
    }
  }
  return matrix;
}

template <typename T, int W, int H>
Matrix<T, W, H>& operator+=(Matrix<T, W, H>& matrixA, const Matrix<T, W, H>& matrixB) {
  for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
      matrixA[x][y] += matrixB[x][y];
    }
  }
  return matrixA;
}

template <typename T, int W, int H>
bool operator<(const Matrix<T, W, H>& matrixA, const Matrix<T, W, H>& matrixB) {
  return std::memcmp(matrixA.m, matrixB.m, sizeof(T) * W * H) < 0;
}

template <typename T, int W, int H>
bool operator==(const Matrix<T, W, H>& matrixA, const Matrix<T, W, H>& matrixB) {
  return 0 == std::memcmp(matrixA.m, matrixB.m, sizeof(T) * W * H);
}

template <typename T, int W, int H>
std::ostream& operator<<(std::ostream& out, const Matrix<T, W, H>& matrix) {
  out << "{";
  for (int y = 0; y < H; ++y) {
    if (y != 0) {
      out << " ";
    }
    out << "{";
    for (int x = 0; x < W; ++x) {
      out << matrix.m[x][y];
      if (x != W - 1) {
        out << ", ";
      }
    }
    out << "}";
    if (y != H - 1) {
      out << "," << std::endl;
    }
  }
  out << "}" << std::endl;
  ;
  return out;
}
};  // namespace pancake
