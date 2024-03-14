#pragma once

#include <cstdint>

#include "object.hpp"
#include "mat.hpp"

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
    vec3 lightDirection{0.f, 0.f, -1.f};

    bool drawMesh = false;

  private:
    void update();
    void draw();
};

