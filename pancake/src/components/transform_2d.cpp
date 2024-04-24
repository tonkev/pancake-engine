#include "components/transform_2d.hpp"

#include "ecs/components.hpp"

using namespace pancake;

Transform2D::Accessor::Accessor(Transform2D& transform) : _transform(transform) {}

Transform2D::Accessor::~Accessor() {
  _transform.ensureLocalClean();
  _transform.ensureGlobalClean();
}

Vec2f& Transform2D::Accessor::translation() const {
  _transform.ensureGlobalClean();
  _transform._state = State::LocalDirty;
  return _transform._translation;
}

Vec2f& Transform2D::Accessor::scale() const {
  _transform.ensureGlobalClean();
  _transform._state = State::LocalDirty;
  return _transform._scale;
}

float& Transform2D::Accessor::rotation() const {
  _transform.ensureGlobalClean();
  _transform._state = State::LocalDirty;
  return _transform._rotation;
}

Vec2f& Transform2D::Accessor::localTranslation() const {
  _transform.ensureLocalClean();
  _transform._state = State::GlobalDirty;
  return _transform._local_translation;
}

Vec2f& Transform2D::Accessor::localScale() const {
  _transform.ensureLocalClean();
  _transform._state = State::GlobalDirty;
  return _transform._local_scale;
}

float& Transform2D::Accessor::localRotation() const {
  _transform.ensureLocalClean();
  _transform._state = State::GlobalDirty;
  return _transform._local_rotation;
}

const Vec2f& Transform2D::translation() const {
  return _translation;
}

const Vec2f& Transform2D::scale() const {
  return _scale;
}

float Transform2D::rotation() const {
  return _rotation;
}

const Vec2f& Transform2D::localTranslation() const {
  return _local_translation;
}

const Vec2f& Transform2D::localScale() const {
  return _local_scale;
}

float Transform2D::localRotation() const {
  return _local_rotation;
}

Mat3f Transform2D::matrix() const {
  return Mat3f::transform(_translation, _rotation, _scale);
}

Mat3f Transform2D::localMatrix() const {
  return Mat3f::transform(_local_translation, _local_rotation, _local_scale);
}

Mat3f Transform2D::inverseMatrix() const {
  return Mat3f::transform(-_translation, -_rotation, _scale.reciprocal());
}

Mat3f Transform2D::inverseLocalMatrix() const {
  return Mat3f::transform(-_local_translation, -_local_rotation, _local_scale.reciprocal());
}

Mat4f Transform2D::matrix3D() const {
  return Mat4f::transform(Vec3f(_translation, 0.f), Vec3f(0.f, 0.f, _rotation), Vec3f(_scale, 1.f));
}

Mat4f Transform2D::localMatrix3D() const {
  return Mat4f::transform(Vec3f(_local_translation, 0.f), Vec3f(0.f, 0.f, _local_rotation),
                          Vec3f(_local_scale, 1.f));
}

Mat4f Transform2D::inverseMatrix3D() const {
  return Mat4f::transform(Vec3f(-_translation, 0.f), Vec3f(0.f, 0.f, -_rotation),
                          Vec3f(_scale.reciprocal(), 1.f));
}

Mat4f Transform2D::inverseLocalMatrix3D() const {
  return Mat4f::transform(Vec3f(-_local_translation, 0.f), Vec3f(0.f, 0.f, -_local_rotation),
                          Vec3f(_local_scale.reciprocal(), 1.f));
}

void Transform2D::setParentGlobalMatrices(const Mat3f& parent_global_transform,
                                          const Mat3f& inv_parent_global_transform) {
  ensureLocalClean();

  _parent_global_transform = parent_global_transform;
  _inv_parent_global_transform = inv_parent_global_transform;

  _state = Transform2D::State::GlobalDirty;
  ensureGlobalClean();
}

void Transform2D::setParentGlobalMatrices(const Transform2D& parent) {
  setParentGlobalMatrices(parent.matrix(), parent.inverseMatrix());
}

Transform2D::Accessor Transform2D::modify() {
  return Accessor(*this);
}

void Transform2D::ensureLocalClean() {
  if (State::LocalDirty == _state) {
    _local_translation = (_inv_parent_global_transform * Vec3f(_translation, 1.0f)).xy();
    _local_scale = _inv_parent_global_transform.getScale().mask(_scale);
    _local_rotation =
        (_inv_parent_global_transform * Mat3f::rotation(_rotation) * Vec3f(1.0f, 0.0f, 0.0f))
            .xy()
            .atan2();

    _state = State::Clean;
  }
}

void Transform2D::ensureGlobalClean() {
  if (State::GlobalDirty == _state) {
    _translation = (_parent_global_transform * Vec3f(_local_translation, 1.0f)).xy();
    _scale = _parent_global_transform.getScale().mask(_local_scale);
    _rotation =
        (_parent_global_transform * Mat3f::rotation(_local_rotation) * Vec3f(1.0f, 0.0f, 0.0f))
            .xy()
            .atan2();

    _state = State::Clean;
  }
}