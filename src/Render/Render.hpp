#pragma once

#include <cstdint>

#include "mat.hpp"
#include "object.hpp"

class Render {
  public:
    Render(uint16_t width, uint16_t height);

    void run();

  private:
    sf::RenderWindow window;
    sf::Clock clock;

    sf::VertexArray drawablesMesh{sf::Lines};
    sf::VertexArray drawables{sf::Triangles};

    mesh object;
    mat4x4 matProj;

    vec3 camera{};
    vec3 lookDir{};
    float yaw;

    bool drawMesh = false;

  private:
    void update(float dt);
    void draw();
};

