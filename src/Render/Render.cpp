#include <cmath>
#include "Render.hpp"

Render::Render(uint16_t width, uint16_t height) {
  window.create(sf::VideoMode(width, height), "Engine3D", sf::Style::Close);
  window.setFramerateLimit(90);

  cube.triangles = {
    // South
    {{ {0.f, 0.f, 0.f},   {0.f, 1.f, 0.f},    {1.f, 1.f, 0.f} }},
    {{ {0.f, 0.f, 0.f},   {1.f, 1.f, 0.f},    {1.f, 0.f, 0.f} }},

    // West
    {{ {0.f, 0.f, 1.f},   {0.f, 1.f, 1.f},    {0.f, 1.f, 0.f} }},
    {{ {0.f, 0.f, 1.f},   {0.f, 1.f, 0.f},    {0.f, 0.f, 0.f} }},

    // North
    {{ {1.f, 0.f, 1.f},   {1.f, 1.f, 1.f},    {0.f, 1.f, 1.f} }},
    {{ {1.f, 0.f, 1.f},   {0.f, 1.f, 1.f},    {0.f, 0.f, 1.f} }},

    // East
    {{ {1.f, 0.f, 0.f},   {1.f, 1.f, 0.f},    {1.f, 1.f, 1.f} }},
    {{ {1.f, 0.f, 0.f},   {1.f, 1.f, 1.f},    {1.f, 0.f, 1.f} }},

    // Top
    {{ {0.f, 1.f, 0.f},   {0.f, 1.f, 1.f},    {1.f, 1.f, 1.f} }},
    {{ {0.f, 1.f, 0.f},   {1.f, 1.f, 1.f},    {1.f, 1.f, 0.f} }},

    // Bottom
    {{ {1.f, 0.f, 1.f},   {0.f, 0.f, 1.f},    {0.f, 0.f, 0.f} }},
    {{ {1.f, 0.f, 1.f},   {0.f, 0.f, 0.f},    {1.f, 0.f, 0.f} }}
  };

  // Projection matrix
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
}

void Render::update() {
  // NOTE: Assuming window size doesn't change
  static const int w = window.getSize().x;
  static const int h = window.getSize().y;
  static float angle = 0.f;

  angle += clock.getElapsedTime().asSeconds();
  clock.restart();
  drawables.clear();

  mat4x4 matRotZ, matRotX;
  matRotZ.rotationZ(angle);
  matRotX.rotationX(angle);

  for (triangle tri : cube.triangles) {
    for (int i = 0; i < 3; i++) {
      sf::Vector3f& p = tri.points[i];

      // Rotate Z
      matRotZ.multiplyTo(p);

      // Rotate X
      matRotX.multiplyTo(p);

      // Translate
      p.z += 3.f;

      // Project
      matProj.multiplyTo(p);

      // Scale into view
      p.x += 1.f; p.y += 1.f; // From [-1, 1] to [0, 2]
      p.x *= 0.5f * w; p.y *= 0.5f * h; // To [0, 1] to screen ranges

      drawables.append(sf::Vector2f{p.x, p.y});
    }
  }

  // Connect the last point with the first point of the last triangle
  drawables.append(drawables[drawables.getVertexCount() - 3]);
}

void Render::draw() {
  window.clear();
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
          default:
            break;
        }
    }

    update();
    draw();
  }
}

