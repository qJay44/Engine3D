#include <cmath>

#include "mat.hpp"

void mat4x4::multiplyTo(vec3& p) const {
  vec3 result;

  // Assuming the 4th component of vector is 1
  result.x = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0] + m[3][0];
  result.y = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1] + m[3][1];
  result.z = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2] + m[3][2];
  float w  = p.x * m[0][3] + p.y * m[1][3] + p.z * m[2][3] + m[3][3]; // The 4th component

  if (w) {
    result.x /= w;
    result.y /= w;
    result.z /= w;
  }

  p = result;
}

mat4x4 getRotationMatX(float angle) {
  mat4x4 mat;

  mat.m[0][0] = 1.f;
  mat.m[1][1] = cosf(angle * 0.5f);
  mat.m[1][2] = sinf(angle * 0.5f);
  mat.m[2][1] = -sinf(angle * 0.5f);
  mat.m[2][2] = cosf(angle * 0.5f);
  mat.m[3][3] = 1.f;

  return mat;
}


mat4x4 getRotationMatZ(float angle) {
  mat4x4 mat;

  mat.m[0][0] = cosf(angle);
  mat.m[0][1] = sinf(angle);
  mat.m[1][0] = -sinf(angle);
  mat.m[1][1] = cosf(angle);
  mat.m[2][2] = 1.f;
  mat.m[3][3] = 1.f;

  return mat;
}

