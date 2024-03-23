#pragma once

#include <cmath>

struct vec2 {
  float x = 0.f;
  float y = 0.f;

  void operator+=(const float& n) {
    x += n;
    y += n;
  }

  void operator*=(const float& n) {
    x *= n;
    y *= n;
  }
};


struct vec3 {
  float x = 0.f;
  float y = 0.f;
  float z = 0.f;

  void operator+=(const float& rhs) {
    x += rhs;
    y += rhs;
    z += rhs;
  }

  void operator+=(const vec3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }

  void operator-=(const vec3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
  }

  void operator*=(const float& rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }

  vec3 operator-(const vec3& rhs) const {
    vec3 result;

    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;

    return result;
  }

  vec3 operator-(const float& rhs) const {
    vec3 result;

    result.x = x - rhs;
    result.y = y - rhs;
    result.z = z - rhs;

    return result;
  }

  vec3 operator+(const vec3& rhs) const {
    vec3 result;

    result.x = x + rhs.x;
    result.y = y + rhs.y;
    result.z = z + rhs.z;

    return result;
  }

  vec3 operator*(const float& rhs) const {
    vec3 result;

    result.x = x * rhs;
    result.y = y * rhs;
    result.z = z * rhs;

    return result;
  }

  vec3 operator*(const vec3& rhs) const {
    vec3 result;

    result.x = x * rhs.x;
    result.y = y * rhs.y;
    result.z = z * rhs.z;

    return result;
  }

  float magnitude() const {
    return sqrtf(x * x + y * y + z * z);
  }

  float dot(const vec3& a) const {
    return x * a.x + y * a.y + z * a.z;
  }

  void normalize() {
    float invMag = 1.f / magnitude();
    *this *= invMag;
  }

  // Cross product
  void cross(const vec3& a, const vec3& b) {
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;
    normalize();
  }
};

static vec3 vecIntersectPlane(const vec3& planeP, vec3& planeN, const vec3& lineStart, const vec3& lineEnd) {
  planeN.normalize();
  float planeD = -planeN.dot(planeP);
  float ad = lineStart.dot(planeN);
  float bd = lineEnd.dot(planeN);
  float t = (-planeD - ad) / (bd - ad);
  vec3 lineStartToEnd = lineEnd - lineStart;
  vec3 lineToIntersect = lineStartToEnd * t;

  return lineStart + lineToIntersect;
}

