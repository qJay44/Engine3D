#pragma once

#include <cstdint>

#include "components.hpp"

class Render {
  public:
    Render(uint16_t width, uint16_t height);

    void run();

  private:
    sf::RenderWindow window;
    sf::Clock clock;

    sf::VertexArray drawables{sf::LineStrip};

    mesh cube;
    mat4x4 matProj;

  private:
    void update();
    void draw();
};

