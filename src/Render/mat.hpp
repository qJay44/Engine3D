#pragma once

#include "vec.hpp"

struct mat4x4 {
  float m[4][4] = {0.f};

  void operator*=(const mat4x4& mat);

  void multiplyTo(vec3& p) const;
};

mat4x4 matMakeRotationX(float angle);
mat4x4 matMakeRotationY(float angle);
mat4x4 matMakeRotationZ(float angle);
mat4x4 matMakeIdentity();
mat4x4 matMakeTranslation(float x, float y, float z);
mat4x4 matMakeProjection(float fov, float aspectRatio, float nearPlane, float farPlane);

mat4x4 matPointAt(const vec3& pos, const vec3& target, const vec3& up);
mat4x4 matQuickInverse(const mat4x4& mat); // Only for Rotation/Translation matrices

