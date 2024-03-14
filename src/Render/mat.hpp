#pragma once

#include "vec.hpp"

struct mat4x4 {
  float m[4][4] = {0.f};

  void multiplyTo(vec3& p) const;
};

mat4x4 getRotationMatX(float angle);
mat4x4 getRotationMatZ(float angle);

