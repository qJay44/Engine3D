#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>

#include "Render.hpp"
#include "object.hpp"

Render::Render(uint16_t width, uint16_t height) {
  static const std::string objectsDir = "../../objects/";

  window.create(sf::VideoMode(width, height), "Engine3D", sf::Style::Close);
  window.setFramerateLimit(90);

  object.loadObj(objectsDir + "mountains.obj");

  float nearPlane = 0.1f;
  float farPlane = 1000.f;
  float fov = 90.f; // theta
  float aspectRatio = (float) height / width;

  matProj = matMakeProjection(fov, aspectRatio, nearPlane, farPlane);
}

void Render::update(float dt) {
  // NOTE: Assuming window size doesn't change
  static const int w = window.getSize().x;
  static const int h = window.getSize().y;
  static float angle = 0.f;

  //angle += dt;
  clock.restart();

  drawablesMesh.clear();
  drawables.clear();

  mat4x4 matWorld = matMakeIdentity();
  mat4x4 matRotZ  = matMakeRotationZ(angle);
  mat4x4 matRotX  = matMakeRotationX(angle);
  mat4x4 matTrans = matMakeTranslation(0.f, 0.f, 8.f);
  mat4x4 matCameraRot = matMakeRotationY(yaw);

  /* matWorld *= matRotZ; */
  /* matWorld *= matRotX; */
  /* matWorld *= matTrans; */

  vec3 up{0.f, 1.f, 0.f};
  vec3 target{0.f, 0.f, 1.f};
  matCameraRot.multiplyTo(target);
  lookDir = target;
  target += camera;

  mat4x4 matCamera = matPointAt(camera, target, up);
  mat4x4 matView = matQuickInverse(matCamera);

  std::vector<triangle> trianglesToRaster;

  for (triangle tri : object.triangles) {

    // Working with 3D space
    for (int i = 0; i < 3; i++)
      matWorld.multiplyTo(tri.points[i]);

    // A normal that points from the triangle surface
    vec3 normal, lineA, lineB;
    lineA = tri.points[1] - tri.points[0];
    lineB = tri.points[2] - tri.points[0];
    normal.cross(lineA, lineB);

    // Projecting to 2D space
    if (normal.dot(tri.points[0] - camera) < 0.f) {

      // Illumination
      {
        vec3 lightDirection{0.f, 1.f, -1.f};
        lightDirection.normalize();
        sf::Uint8 c = normal.dot(lightDirection) * 255.f;
        tri.color = {c, c, c};
      }

      // Convert World Space --> View Space
      for (int i = 0; i < 3; i++)
        matView.multiplyTo(tri.points[i]);

      // Clipping viewed trinangle against near plane
      int clippedTriangles = 0;
      triangle clipped[2];
      clippedTriangles = triangleClipAgainstPlane({0.f, 0.f, 0.1f}, {0.f, 0.f, 1.f}, tri, clipped[0], clipped[1]);

      for (int n = 0; n < clippedTriangles; n++) {
        // Project (3D -> 2D)
        for (int i = 0; i < 3; i++) {
          vec3& p = clipped[n].points[i];
          matProj.multiplyTo(p);

          // Scale into view
          p.x *= -1.f; p.y *= -1.f; // X and Y are inverted so put them back
          p.x += 1.f;  p.y += 1.f; // From [-1, 1] to [0, 2]
          p.x *= 0.5f; p.y *= 0.5f; // To [0, 1]
          p.x *= w; p.y *= h; // To screen ranges
        }
        tri.color = clipped[n].color;
        trianglesToRaster.push_back(clipped[n]);
      }
    }
  }

  // Sort triangles based on "z" value
  std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](const triangle& t1, const triangle& t2) {
    float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.f;
    float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.f;

    return z1 < z2;
  });

  for (triangle& tri : trianglesToRaster) {
    triangle clipped[2];
    std::list<triangle> listTriangles;
    listTriangles.push_back(tri);
    int newTriangles = 1;

    // Clip against all four screen edges
    for (int p = 0; p < 4; p++) {
      int trisToAdd = 0;
      while (newTriangles > 0) {
        triangle test = listTriangles.front();
        listTriangles.pop_front();
        newTriangles--;

        switch (p) {
          case 0: trisToAdd = triangleClipAgainstPlane({0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, test, clipped[0], clipped[1]); break;
          case 1: trisToAdd = triangleClipAgainstPlane({0.f, h - 1.f, 0.f}, {0.f, -1.f, 0.f}, test, clipped[0], clipped[1]); break;
          case 2: trisToAdd = triangleClipAgainstPlane({0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, test, clipped[0], clipped[1]); break;
          case 3: trisToAdd = triangleClipAgainstPlane({w - 1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f}, test, clipped[0], clipped[1]); break;
        }

        for (int n = 0; n < trisToAdd; n++)
          listTriangles.push_back(clipped[n]);
      }
      newTriangles = listTriangles.size();
    }

    // Create drawable vertices of each triangle
    for (const triangle& t : listTriangles) {
      for (int i = 0; i < 3; i++) {
        const vec3& p = tri.points[i];
        sf::Vertex vert{{p.x, p.y}, tri.color};
        sf::Vertex vertMesh{{p.x, p.y}, sf::Color::White};

        drawables.append(vert);
        drawablesMesh.append(vertMesh);

        // Append the point again as a start for the next line (except first one)
        if (i) drawablesMesh.append(vertMesh);
      }
      // Connect last appended point with first one
      drawablesMesh.append({{tri.points[0].x, tri.points[0].y}, sf::Color::White});
    }
  }
}

void Render::draw() {
  window.clear();

  window.draw(drawables);

  if (drawMesh)
    window.draw(drawablesMesh);

  window.display();
}

void Render::run() {
  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();
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

    vec3 forward = lookDir * 8.f * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    camera.y += 8.f * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  camera.y -= 8.f * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  camera.x -= 8.f * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) camera.x += 8.f * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) camera += forward;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) camera -= forward;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) yaw -= 2.f * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) yaw += 2.f * dt;

    update(dt);
    draw();
  }
}

