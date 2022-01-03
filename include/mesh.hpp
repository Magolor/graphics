#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

class Mesh : public Object3D {
public:
    std::vector<Triangle> A;
    Mesh(const char *filename, Material *material);
    bool intersect(const Ray &r, Hit &h, double tmin) override;
};

#endif