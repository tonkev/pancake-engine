#pragma once

#include "util/matrix.hpp"
#include "util/type_desc_library.hpp"

#include <cmath>
#include <limits>

namespace pancake {
template <typename T>
class Quaternion {
 public:
  Quaternion() : x(0), y(0), z(0), w(1) {}
  Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { normalise(); }
  Quaternion(T angle, const Matrix<T, 1, 3>& axis) {
    T sin = std::sin(angle / (T)2);
    w = std::cos(angle / (T)2);
    x = sin * axis.x();
    y = sin * axis.y();
    z = sin * axis.z();
    normalise();
  }
  // assumes pure rotation matrix
  Quaternion(const Matrix<T, 4, 4>& matrix) {
    T t;
    if (matrix[2][2] < 0) {
      if (matrix[0][0] > matrix[1][1]) {
        t = 1 + matrix[0][0] - matrix[1][1] - matrix[2][2];
        x = t;
        y = matrix[0][1] + matrix[1][0];
        z = matrix[2][0] + matrix[0][2];
        w = matrix[1][2] - matrix[2][1];
      } else {
        t = 1 - matrix[0][0] + matrix[1][1] - matrix[2][2];
        x = matrix[0][1] + matrix[1][0];
        y = t;
        z = matrix[1][2] + matrix[2][1];
        w = matrix[2][0] - matrix[0][2];
      }
    } else {
      if (matrix[0][0] < -matrix[1][1]) {
        t = 1 - matrix[0][0] - matrix[1][1] + matrix[2][2];
        x = matrix[2][0] + matrix[0][2];
        y = matrix[1][2] + matrix[2][1];
        z = t;
        w = matrix[0][1] - matrix[1][0];
      } else {
        t = 1 + matrix[0][0] + matrix[1][1] + matrix[2][2];
        x = matrix[1][2] - matrix[2][1];
        y = matrix[2][0] - matrix[0][2];
        z = matrix[0][1] - matrix[1][0];
        w = t;
      }
    }

    if constexpr (std::is_same_v<T, float>) {
      (*this) *= 0.5f / sqrtf(t);
    } else {
      (*this) *= 0.5 / sqrt(t);
    }

    normalise();
  }

  T norm() const {
    T a = (x * x) + (y * y) + (z * z) + (w * w);
    return static_cast<float>(std::sqrt(a));
  }

  Quaternion<T>& normalise() {
    T n = 1 / norm();
    x *= n;
    y *= n;
    z *= n;
    w *= n;
    return *this;
  }

  Quaternion<T> normalised() {
    T n = 1 / norm();
    return Quaternion<T>(*this) * n;
  }

  Quaternion<T> conjugate() const { return Quaternion<T>(-x, -y, -z, w); }

  Quaternion<T> inverse() const {
    T n = norm();
    return ((((T)1) / (n * n)) * conjugate()).normalised();
  }

  Matrix<T, 4, 4> matrix() const {
    T s = ((T)2) / norm();
    return Matrix<T, 4, 4>({((T)1) - (s * ((y * y) + (z * z))), s * ((x * y) - (w * z)),
                            s * ((x * z) + (w * y)), (T)0, s * ((x * y) + (w * z)),
                            ((T)1) - (s * ((x * x) + (z * z))), s * ((y * z) - (w * x)), (T)0,
                            s * ((x * z) - (w * y)), s * ((y * z) + (w * x)),
                            ((T)1) - (s * ((x * x) + (y * y))), (T)0, (T)0, (T)0, (T)0, (T)1});
  }

  void swingTwist(const Matrix<T, 1, 3>& twist_axis, Quaternion<T>& swing, Quaternion<T>& twist) {
    Matrix<T, 1, 3> r(x, y, z);
    if (r.squaredNorm() < std::numeric_limits<T>::epsilon()) {
      Matrix<T, 1, 3> rotated_twist_axis =
          (matrix() * Matrix<T, 1, 4>(twist_axis.x(), twist_axis.y(), twist_axis.z(), 0)).xyz();
      Matrix<T, 1, 3> swing_axis = twist_axis.cross(rotated_twist_axis);

      if (swing_axis.squaredNorm() > std::numeric_limits<T>::epsilon()) {
        T swing_angle = twist_axis.angle(rotated_twist_axis);
        swing = Quaternion<T>(swing_angle, swing_axis);
      } else {
        swing = Quaternion<T>::identity();
      }

      twist = Quaternion<T>(3.14f, twist_axis);
      return;
    }

    Matrix<T, 1, 3> p = r.dot(twist_axis) * twist_axis;
    twist = Quaternion<T>(p.x(), p.y(), p.z(), w);
    swing = (*this) * twist.inverse();
  }

  void angleAxis(T& angle, Matrix<T, 1, 3>& axis) {
    T s = std::sqrt(1 - (w * w));
    angle = 2 * std::acos(w);

    if ((-0.001 < s) && (s < 0.001)) {
      axis = Matrix<T, 1, 3>::zeros();
    } else {
      axis.x() = x / s;
      axis.y() = y / s;
      axis.z() = z / s;
    }
  }

  static Quaternion<T> identity() { return Quaternion<T>(); }

  T x;
  T y;
  T z;
  T w;

 private:
  static inline const TypeDesc& DESC
#ifdef __linux__
      __attribute__((used))
#endif
      = TypeDescLibrary::get<Quaternion<T>>()
            .addField("x", 0, TypeDescLibrary::get<T>())
            .addField("y", sizeof(T), TypeDescLibrary::get<T>())
            .addField("z", 2 * sizeof(T), TypeDescLibrary::get<T>())
            .addField("w", 3 * sizeof(T), TypeDescLibrary::get<T>());
};

template <typename T>
Quaternion<T> operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs) {
  return Quaternion<T>((lhs.y * rhs.z) - (lhs.z * rhs.y) + (rhs.w * lhs.x) + (lhs.w * rhs.x),
                       (lhs.z * rhs.x) - (lhs.x * rhs.z) + (rhs.w * lhs.y) + (lhs.w * rhs.y),
                       (lhs.x * rhs.y) - (lhs.y * rhs.x) + (rhs.w * lhs.z) + (lhs.w * rhs.z),
                       (lhs.w * rhs.w) - (lhs.x * rhs.x) - (rhs.y * lhs.y) - (lhs.z * rhs.z));
}

template <typename T>
Quaternion<T> operator*(const Quaternion<T>& quaternion, const T scalar) {
  return Quaternion<T>(quaternion.x * scalar, quaternion.y * scalar, quaternion.z * scalar,
                       quaternion.w * scalar);
}

template <typename T>
Quaternion<T> operator*(const T scalar, const Quaternion<T>& matrix) {
  return matrix * scalar;
}

template <typename T>
Quaternion<T>& operator*=(Quaternion<T>& lhs, const Quaternion<T>& rhs) {
  T x = (lhs.y * rhs.z) - (lhs.z * rhs.y) + (rhs.w * lhs.x) + (lhs.w * rhs.x);
  T y = (lhs.z * rhs.x) - (lhs.x * rhs.z) + (rhs.w * lhs.y) + (lhs.w * rhs.y);
  T z = (lhs.x * rhs.y) - (lhs.y * rhs.x) + (rhs.w * lhs.z) + (lhs.w * rhs.z);
  T w = (lhs.w * rhs.w) - (lhs.x * rhs.x) - (rhs.y * lhs.y) - (lhs.z * rhs.z);
  lhs.x = x;
  lhs.y = y;
  lhs.z = z;
  lhs.w = w;
  return lhs.normalise();
}

template <typename T>
Quaternion<T>& operator*=(Quaternion<T>& lhs, const T& rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  lhs.w *= rhs;
  return lhs;
}

typedef Quaternion<float> QuaternionF;
}  // namespace pancake