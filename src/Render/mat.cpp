#include <cmath>

#include "mat.hpp"

void mat4x4::operator*=(const mat4x4& mat) {
  mat4x4 result;

  for (int c = 0; c < 4; c++)
    for (int r = 0; r < 4; r++)
      result.m[r][c] =
        m[r][0] * mat.m[0][c] +
        m[r][1] * mat.m[1][c] +
        m[r][2] * mat.m[2][c] +
        m[r][3] * mat.m[3][c];

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      m[i][j] = result.m[i][j];
}

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

mat4x4 matMakeRotationX(float angle) {
  mat4x4 mat;

  mat.m[0][0] = 1.f;
  mat.m[1][1] = cosf(angle * 0.5f);
  mat.m[1][2] = sinf(angle * 0.5f);
  mat.m[2][1] = -sinf(angle * 0.5f);
  mat.m[2][2] = cosf(angle * 0.5f);
  mat.m[3][3] = 1.f;

  return mat;
}

mat4x4 matMakeRotationY(float angle) {
  mat4x4 mat;

  mat.m[0][0] = cosf(angle);
  mat.m[0][2] = sinf(angle);
  mat.m[2][0] = -sinf(angle);
  mat.m[1][1] = 1.f;
  mat.m[2][2] = cosf(angle);
  mat.m[3][3] = 1.f;

  return mat;

}

mat4x4 matMakeRotationZ(float angle) {
  mat4x4 mat;

  mat.m[0][0] = cosf(angle);
  mat.m[0][1] = sinf(angle);
  mat.m[1][0] = -sinf(angle);
  mat.m[1][1] = cosf(angle);
  mat.m[2][2] = 1.f;
  mat.m[3][3] = 1.f;

  return mat;
}

mat4x4 matMakeIdentity() {
  mat4x4 mat;

  mat.m[0][0] = 1.f;
  mat.m[1][1] = 1.f;
  mat.m[2][2] = 1.f;
  mat.m[3][3] = 1.f;

  return mat;
}

mat4x4 matMakeTranslation(float x, float y, float z) {
  mat4x4 mat;

  mat.m[0][0] = 1.f;
  mat.m[1][1] = 1.f;
  mat.m[2][2] = 1.f;
  mat.m[3][3] = 1.f;
  mat.m[3][0] = x;
  mat.m[3][1] = y;
  mat.m[3][2] = z;

  return mat;
}

mat4x4 matMakeProjection(float fov, float aspectRatio, float nearPlane, float farPlane) {
  mat4x4 mat;

  float fovRad = 1.f / tanf(fov * 0.5 / 180.f * 3.14159f);

  mat.m[0][0] = aspectRatio * fovRad;
  mat.m[1][1] = fovRad;
  mat.m[2][2] = farPlane / (farPlane - nearPlane);
  mat.m[2][3] = 1.f;
  mat.m[3][2] = (farPlane * nearPlane) / (farPlane - nearPlane);

  return mat;
}

mat4x4 matPointAt(const vec3& pos, const vec3& target, const vec3& up) {
  vec3 newForward = target - pos;
  newForward.normalize();

  vec3 a = newForward * up.dot(newForward);
  vec3 newUp = up - a;
  newUp.normalize();

  vec3 newRight;
  newRight.cross(newUp, newForward);

  mat4x4 mat{
    newRight.x  , newRight.y  , newRight.z  , 0.f,
    newUp.x     , newUp.y     , newUp.z     , 0.f,
    newForward.x, newForward.y, newForward.z, 0.f,
    pos.x       , pos.y       , pos.z       , 1.f,
  };

  return mat;
}

mat4x4 matQuickInverse(const mat4x4& mat) {
  float invPosX = -(mat.m[3][0] * mat.m[0][0] + mat.m[3][1] * mat.m[0][1] + mat.m[3][2] * mat.m[0][2]);
  float invPosY = -(mat.m[3][0] * mat.m[1][0] + mat.m[3][1] * mat.m[1][1] + mat.m[3][2] * mat.m[1][2]);
  float invPosZ = -(mat.m[3][0] * mat.m[2][0] + mat.m[3][1] * mat.m[2][1] + mat.m[3][2] * mat.m[2][2]);

  mat4x4 result{
    mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.f,
    mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.f,
    mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.f,
    invPosX, invPosY, invPosZ, 1.f
  };

  return result;
}

