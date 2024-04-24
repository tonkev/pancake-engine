#include "components/transform_3d.hpp"

#include "ecs/components.hpp"

using namespace pancake;

Transform3D::Accessor::Accessor(Transform3D& transform) : _transform(transform) {}

Transform3D::Accessor::~Accessor() {
  _transform.ensureLocalClean();
  _transform.ensureGlobalClean();
}

Vec3f& Transform3D::Accessor::translation() const {
  _transform.ensureGlobalClean();
  _transform._state = State::LocalDirty;
  return _transform._translation;
}

Vec3f& Transform3D::Accessor::scale() const {
  _transform.ensureGlobalClean();
  _transform._state = State::LocalDirty;
  return _transform._scale;
}

QuaternionF& Transform3D::Accessor::rotation() const {
  _transform.ensureGlobalClean();
  _transform._state = State::LocalDirty;
  return _transform._rotation;
}

Vec3f& Transform3D::Accessor::localTranslation() const {
  _transform.ensureLocalClean();
  _transform._state = State::GlobalDirty;
  return _transform._local_translation;
}

Vec3f& Transform3D::Accessor::localScale() const {
  _transform.ensureLocalClean();
  _transform._state = State::GlobalDirty;
  return _transform._local_scale;
}

QuaternionF& Transform3D::Accessor::localRotation() const {
  _transform.ensureLocalClean();
  _transform._state = State::GlobalDirty;
  return _transform._local_rotation;
}

const Vec3f& Transform3D::translation() const {
  return _translation;
}

const Vec3f& Transform3D::scale() const {
  return _scale;
}

QuaternionF Transform3D::rotation() const {
  return _rotation;
}

const Vec3f& Transform3D::localTranslation() const {
  return _local_translation;
}

const Vec3f& Transform3D::localScale() const {
  return _local_scale;
}

QuaternionF Transform3D::localRotation() const {
  return _local_rotation;
}

Mat4f Transform3D::matrix() const {
  return Mat4f::translation(_translation) * _rotation.matrix() * Mat4f::scale(_scale);
}

Mat4f Transform3D::localMatrix() const {
  return Mat4f::translation(_local_translation) * _local_rotation.matrix() *
         Mat4f::scale(_local_scale);
}

Mat4f Transform3D::inverseMatrix() const {
  return Mat4f::scale(_scale.reciprocal()) * _rotation.inverse().matrix() *
         Mat4f::translation(-_translation);
}

Mat4f Transform3D::inverseLocalMatrix() const {
  return Mat4f::scale(_local_scale.reciprocal()) * _local_rotation.inverse().matrix() *
         Mat4f::translation(-_local_translation);
}

void Transform3D::setParentGlobalMatrices(const Mat4f& parent_global_transform,
                                          const Mat4f& inv_parent_global_transform) {
  ensureLocalClean();

  _parent_global_transform = parent_global_transform;
  _inv_parent_global_transform = inv_parent_global_transform;

  _state = Transform3D::State::GlobalDirty;
  ensureGlobalClean();
}

void Transform3D::setParentGlobalMatrices(const Transform3D& parent) {
  setParentGlobalMatrices(parent.matrix(), parent.inverseMatrix());
}

Transform3D::Accessor Transform3D::modify() {
  return Accessor(*this);
}

void Transform3D::ensureLocalClean() {
  if (State::LocalDirty == _state) {
    _local_translation = (_inv_parent_global_transform * Vec4f(_translation, 1.0f)).xyz();
    _local_scale = _inv_parent_global_transform.getScale().mask(_scale);
    _local_rotation = QuaternionF(_inv_parent_global_transform * _rotation.matrix());

    _state = State::Clean;
  }
}

void Transform3D::ensureGlobalClean() {
  if (State::GlobalDirty == _state) {
    _translation = (_parent_global_transform * Vec4f(_local_translation, 1.0f)).xyz();
    _scale = _parent_global_transform.getScale().mask(_local_scale);
    _rotation = QuaternionF(_parent_global_transform * _local_rotation.matrix());

    _state = State::Clean;
  }
}