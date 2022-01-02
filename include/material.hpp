#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;

class Material {
public:
    Vector4f albedo;
    Vector3f diffuseColor;
    Vector3f specularColor;
    double refractivity, shininess;

    explicit Material(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, double s = 0, double r = 1, const Vector4f &a = Vector4f::IND0+Vector4f::IND1)
        : diffuseColor(d_color), specularColor(s_color), shininess(s), refractivity(r), albedo(a) {}
    explicit Material(const char *filename) {
        std::ifstream f;
        f.open(filename);
        if (!f.is_open()) {
            std::cout << "Cannot open " << filename << "\n";
            return;
        }
        std::string line;
        std::string tok;
        while(true) {
            std::getline(f, line);
            if (f.eof()) {
                break;
            }
            if (line.size() < 3) {
                continue;
            }
            if (line.at(0) == '#') {
                continue;
            }
            std::stringstream ss(line);
            ss >> tok;
            if (tok == "albedo") {
                ss >> albedo[0] >> albedo[1] >> albedo[2] >> albedo[3];
            } else if (tok == "diffuseColor") {
                ss >> diffuseColor[0] >> diffuseColor[1] >> diffuseColor[2];
            } else if (tok == "specularColor") {
                ss >> specularColor[0] >> specularColor[1] >> specularColor[2];
            } else if (tok == "refractivity") {
                ss >> refractivity;
            } else if (tok == "shininess") {
                ss >> shininess;
            }
        }
        f.close();
    }
    virtual ~Material() = default;

    Vector3f Shade(const Ray &ray, const Hit &hit, const Vector3f &dirToLight, const Vector3f &lightColor);
};

#endif // MATERIAL_H