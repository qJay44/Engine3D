#pragma once

#include <cstdio>
#include <fstream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "vec.hpp"

struct triangle {
  vec3 points[3];
  sf::Color color{};
};

struct mesh {
  std::list<triangle> triangles;

  void loadObj(std::string fileName) {
    std::ifstream file(fileName);
    if (!file.is_open())
      printf("Can't open the file: %s\n", fileName.c_str());

    std::vector<vec3> verts;

    while (!file.eof()) {
      char line[128];
      file.getline(line, 128);

      std::stringstream s;
      s << line;

      char junk;

      if (line[0] == 'v') {
        vec3 v;
        s >> junk >> v.x >> v.y >> v.z;
        verts.push_back(v);
      }

      if (line[0] == 'f') {
        int f[3];
        s >> junk >> f[0] >> f[1] >> f[2];
        triangles.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});
      }
    }

    printf("File was succsefully loaded\n");
  }
};

static int triangleClipAgainstPlane(vec3 planeP, vec3 planeN, triangle& inTri, triangle& outTri1, triangle& outTri2) {
  planeN.normalize();

  auto dist = [&](vec3 p) {
    p.normalize();
    return planeN.x * p.x + planeN.y * p.y + planeN.z * p.z - planeN.dot(planeP);
  };

  vec3* insidePoints[3]; int insidePointsCount = 0;
  vec3* outsidePoints[3]; int outsidePointsCount = 0;

  float d0 = dist(inTri.points[0]);
  float d1 = dist(inTri.points[1]);
  float d2 = dist(inTri.points[2]);

  if (d0 >= 0.f) insidePoints[insidePointsCount++] = &inTri.points[0];
  else outsidePoints[outsidePointsCount++] = &inTri.points[0];

  if (d1 >= 0.f) insidePoints[insidePointsCount++] = &inTri.points[1];
  else outsidePoints[outsidePointsCount++] = &inTri.points[1];

  if (d2 >= 0.f) insidePoints[insidePointsCount++] = &inTri.points[2];
  else outsidePoints[outsidePointsCount++] = &inTri.points[2];

  switch (insidePointsCount) {
    case 0:
      return 0;
    case 3:
      outTri1 = inTri;
      return 1;
    case 1:
      if (outsidePointsCount == 2) {
        outTri1.color = sf::Color::Blue; // inTri.color

        outTri1.points[0] = *insidePoints[0];
        outTri1.points[1] = vecIntersectPlane(planeP, planeN, *insidePoints[0], *outsidePoints[0]);
        outTri1.points[2] = vecIntersectPlane(planeP, planeN, *insidePoints[0], *outsidePoints[1]);

        return 1;
      }
      break;
    case 2:
      if (outsidePointsCount == 1) {
        outTri1.color = sf::Color::Green; // inTri.color
        outTri2.color = sf::Color::Red; // inTri.color

        outTri1.points[0] = *insidePoints[0];
        outTri1.points[1] = *insidePoints[1];
        outTri1.points[2] = vecIntersectPlane(planeP, planeN, *insidePoints[0], *outsidePoints[0]);

        outTri2.points[0] = *insidePoints[1];
        outTri2.points[1] = outTri1.points[2];
        outTri2.points[2] = vecIntersectPlane(planeP, planeN, *insidePoints[1], *outsidePoints[0]);

        return 2;
      }
      break;
  }

  // Should never happen
  throw std::runtime_error("triangleClipAgainstPlane: nothing to return");
}

