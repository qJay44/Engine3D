#include <algorithm>
#include <cmath>
#include "Render.hpp"

Render::Render(uint16_t width, uint16_t height) {
  window.create(sf::VideoMode(width, height), "Engine3D", sf::Style::Close);
  window.setFramerateLimit(90);

  object.loadObj("spaceship.obj");

  // Projection matrix //

  float nearPlane = 0.1f;
  float farPlane = 1000.f;
  float fov = 90.f; // theta
  float aspectRatio = (float) height / width;
  float fovRad = 1.f / tanf(fov * 0.5 / 180.f * 3.14159f);

  matProj.m[0][0] = aspectRatio * fovRad;
  matProj.m[1][1] = fovRad;
  matProj.m[2][2] = farPlane / (farPlane - nearPlane);
  matProj.m[2][3] = 1.f;
  matProj.m[3][2] = (farPlane * nearPlane) / (farPlane - nearPlane);

  lightDirection.normalize();

  ///////////////////////
}

void Render::update() {
  // NOTE: Assuming window size doesn't change
  static const int w = window.getSize().x;
  static const int h = window.getSize().y;
  static float angle = 0.f;

  angle += clock.getElapsedTime().asSeconds();
  clock.restart();

  drawablesMesh.clear();
  drawables.clear();

  mat4x4 matRotZ, matRotX;
  matRotZ = getRotationMatZ(angle);
  matRotX = getRotationMatX(angle);

  std::vector<triangle> trianglesToRaster;

  for (triangle tri : object.triangles) {

    // Working with 3D space
    for (int i = 0; i < 3; i++) {
      vec3& p = tri.points[i];

      // Rotate Z
      matRotZ.multiplyTo(p);

      // Rotate X
      matRotX.multiplyTo(p);

      // Translate
      p.z += 8.f;
    }

    // A normal that points from the triangle surface
    vec3 normal, lineA, lineB;
    lineA = tri.points[1] - tri.points[0];
    lineB = tri.points[2] - tri.points[0];
    normal.cross(lineA, lineB);

    // Projecting to 2D space
    if (normal.dot(tri.points[0] - camera) < 0.f) {
      sf::Uint8 alpha = normal.dot(lightDirection) * 255.f;

      for (int i = 0; i < 3; i++) {
        vec3& p = tri.points[i];

        // Project (3D -> 2D)
        matProj.multiplyTo(p);

        // Scale into view
        p += 1.f; // From [-1, 1] to [0, 2]
        p *= 0.5f; // To [0, 1]
        p.x *= w; p.y *= h; // To screen ranges

        tri.color = {255, 255, 255, alpha};
      }

      trianglesToRaster.push_back(tri);
    }
  }

  // Sort triangles based on "z" value
  std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](const triangle& t1, const triangle& t2) {
    float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.f;
    float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.f;

    return z1 > z2;
  });

  // Create vertices of triangle
  for (const triangle& tri : trianglesToRaster) {
    for (int i = 0; i < 3; i++) {
      const vec3& p = tri.points[i];
      sf::Vertex vert{{p.x, p.y}, tri.color};

      drawablesMesh.append(vert);
      drawables.append(vert);

      // Append the point again as a start for the next line (except first one)
      if (i) drawablesMesh.append(vert);
    }
  }

  // Connect last point with first point
  drawablesMesh.append(drawablesMesh[drawablesMesh.getVertexCount() - 5]);
}

void Render::draw() {
  window.clear();

  if (drawMesh)
    window.draw(drawablesMesh);

  window.draw(drawables);

  window.display();
}

void Render::run() {
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::KeyReleased)
        switch (event.key.code) {
          case sf::Keyboard::Q:
            window.close();
            break;
          case sf::Keyboard::M:
            drawMesh = !drawMesh;
            break;
          default:
            break;
        }
    }

    update();
    draw();
  }
}

