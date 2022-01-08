#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "material.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <unordered_map>
#include "kdtree.hpp"

class Mesh : public Object3D {
public:
    std::vector<Triangle> A; std::unordered_map<string,Material*> M; KDTree *T;
    Mesh(const char *filename, Material *material);
    Mesh(vector<Triangle> *triangles, Material *material);
    bool intersect(const Ray &r, Hit &h, double tmin) override;
};

#endif