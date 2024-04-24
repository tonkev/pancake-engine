#pragma once

#include "util/matrix.hpp"
#include "util/pstruct.hpp"

namespace pancake {
class PropagateTransforms;

PSTRUCT(Transform2D)
public:
struct Accessor {
 public:
  Accessor(Transform2D& transform);
  ~Accessor();

  Vec2f& translation() const;
  Vec2f& scale() const;
  float& rotation() const;

  Vec2f& localTranslation() const;
  Vec2f& localScale() const;
  float& localRotation() const;

 private:
  Transform2D& _transform;
};

const Vec2f& translation() const;
const Vec2f& scale() const;
float rotation() const;

const Vec2f& localTranslation() const;
const Vec2f& localScale() const;
float localRotation() const;

Mat3f matrix() const;
Mat3f localMatrix() const;

Mat3f inverseMatrix() const;
Mat3f inverseLocalMatrix() const;

Mat4f matrix3D() const;
Mat4f localMatrix3D() const;

Mat4f inverseMatrix3D() const;
Mat4f inverseLocalMatrix3D() const;

void setParentGlobalMatrices(const Mat3f& parent_global_transform,
                             const Mat3f& inv_parent_global_transform);
void setParentGlobalMatrices(const Transform2D& parent);

Accessor modify();

private:
enum class State { Clean, LocalDirty, GlobalDirty };

void ensureLocalClean();
void ensureGlobalClean();

PSTRUCT_MEMBER_INITIALISED(Vec2f, _translation, Vec2f::zeros())
PSTRUCT_MEMBER_INITIALISED(Vec2f, _scale, Vec2f::ones())
PSTRUCT_MEMBER_INITIALISED(float, _rotation, 0.f)

PSTRUCT_MEMBER_INITIALISED(Vec2f, _local_translation, Vec2f::zeros())
PSTRUCT_MEMBER_INITIALISED(Vec2f, _local_scale, Vec2f::ones())
PSTRUCT_MEMBER_INITIALISED(float, _local_rotation, 0.f)

PSTRUCT_MEMBER_INITIALISED(Mat3f, _parent_global_transform, Mat3f::identity())
PSTRUCT_MEMBER_INITIALISED(Mat3f, _inv_parent_global_transform, Mat3f::identity())

PSTRUCT_MEMBER_INITIALISED(State, _state, State::Clean)

friend Accessor;
friend PropagateTransforms;
PSTRUCT_END()
}  // namespace pancake