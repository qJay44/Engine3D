#include <cmath>
#include <list>

struct triangle {
  sf::Vector3f points[3];
};

struct mesh {
  std::list<triangle> triangles;
};

struct mat4x4 {
  float m[4][4] = {0.f};

  void multiplyTo(sf::Vector3f& p) const {
    sf::Vector3f result;

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

  void rotationX(float angle) {
    m[0][0] = 1.f;
    m[1][1] = cosf(angle * 0.5f);
    m[1][2] = sinf(angle * 0.5f);
    m[2][1] = -sinf(angle * 0.5f);
    m[2][2] = cosf(angle * 0.5f);
    m[3][3] = 1.f;
  }

  void rotationZ(float angle) {
    m[0][0] = cosf(angle);
    m[0][1] = sinf(angle);
    m[1][0] = -sinf(angle);
    m[1][1] = cosf(angle);
    m[2][2] = 1.f;
    m[3][3] = 1.f;
  }
};

