#pragma once

#include <cmath>

struct vec2 {
  float x, y;

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
  float x, y, z;

  void operator+=(const float& n) {
    x += n;
    y += n;
    z += n;
  }

  void operator*=(const float& n) {
    x *= n;
    y *= n;
    z *= n;
  }

  vec3 operator-(const vec3& rhs) const {
    vec3 result;

    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;

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

