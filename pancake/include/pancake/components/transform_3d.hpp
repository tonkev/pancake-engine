#pragma once

#include "math/quaternion.hpp"
#include "util/matrix.hpp"
#include "util/pstruct.hpp"

namespace pancake {
class PropagateTransform3D;

PSTRUCT(Transform3D) public : struct Accessor {
 public:
  Accessor(Transform3D& transform);
  ~Accessor();

  Vec3f& translation() const;
  Vec3f& scale() const;
  QuaternionF& rotation() const;

  Vec3f& localTranslation() const;
  Vec3f& localScale() const;
  QuaternionF& localRotation() const;

 private:
  Transform3D& _transform;
};

const Vec3f& translation() const;
const Vec3f& scale() const;
QuaternionF rotation() const;

const Vec3f& localTranslation() const;
const Vec3f& localScale() const;
QuaternionF localRotation() const;

Mat4f matrix() const;
Mat4f localMatrix() const;

Mat4f inverseMatrix() const;
Mat4f inverseLocalMatrix() const;

void setParentGlobalMatrices(const Mat4f& parent_global_transform,
                             const Mat4f& inv_parent_global_transform);
void setParentGlobalMatrices(const Transform3D& parent);

Accessor modify();

private:
enum class State { Clean, LocalDirty, GlobalDirty };

void ensureLocalClean();
void ensureGlobalClean();

PSTRUCT_MEMBER_INITIALISED(Vec3f, _translation, Vec3f::zeros())
PSTRUCT_MEMBER_INITIALISED(Vec3f, _scale, Vec3f::ones())
PSTRUCT_MEMBER_INITIALISED(QuaternionF, _rotation, QuaternionF::identity())

PSTRUCT_MEMBER_INITIALISED(Vec3f, _local_translation, Vec3f::zeros())
PSTRUCT_MEMBER_INITIALISED(Vec3f, _local_scale, Vec3f::ones())
PSTRUCT_MEMBER_INITIALISED(QuaternionF, _local_rotation, QuaternionF::identity())

PSTRUCT_MEMBER_INITIALISED(Mat4f, _parent_global_transform, Mat4f::identity())
PSTRUCT_MEMBER_INITIALISED(Mat4f, _inv_parent_global_transform, Mat4f::identity())

PSTRUCT_MEMBER_INITIALISED(State, _state, State::Clean)

friend Accessor;
friend PropagateTransform3D;
PSTRUCT_END()
}  // namespace pancake