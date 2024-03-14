#pragma once

#include <cstdio>
#include <fstream>
#include <list>
#include <sstream>
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
      printf("Can't open the file: %s", fileName.c_str());

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

    printf("File was succsefully loaded");
  }
};

