#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "image.hpp"
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
    Vector3f ambienceColor;
    double refractivity, shininess, portalness;
    Image *texture;

    explicit Material(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &a_color = Vector3f::ZERO, double s = 0, double r = 1, const Vector4f &a = Vector4f::IND0+Vector4f::IND1, const char *filename = nullptr)
        : diffuseColor(d_color), specularColor(s_color), ambienceColor(a_color), shininess(s), portalness(0.), refractivity(r), albedo(a), texture(nullptr) {if(filename!=nullptr&&filename[0]) texture = Image::LoadTGA(filename);}
    explicit Material(const Vector3f &d_color = Vector3f::ZERO, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &a_color = Vector3f::ZERO, double s = 0, double r = 1, const Vector4f &a = Vector4f::IND0+Vector4f::IND1, Image *texture = nullptr)
        : diffuseColor(d_color), specularColor(s_color), ambienceColor(a_color), shininess(s), portalness(0.), refractivity(r), albedo(a), texture(texture) {}
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
            } else if (tok == "ambienceColor") {
                ss >> ambienceColor[0] >> ambienceColor[1] >> ambienceColor[2];
            } else if (tok == "refractivity") {
                ss >> refractivity;
            } else if (tok == "shininess") {
                ss >> shininess;
            } else if (tok == "portalness") {
                ss >> portalness;
            }
        }
        f.close();
        texture = nullptr;
    }
    virtual ~Material() = default;

    Vector3f Shade(const Ray &ray, const Hit &hit, const Vector3f &dirToLight, const Vector3f &lightColor, bool shadow);
    Vector3f getNearestCyclicTexture(Vector2f texcoord) const {
        if(texture==nullptr) return diffuseColor; double x = texcoord[0]*(texture->w-1), y = texcoord[1]*(texture->h-1); int lx = floor(x), ly = floor(y), rx = ceil(x), ry = ceil(y);
        double w0 = Vector2f(x-lx,y-ly).length(), w1 = Vector2f(x-lx,ry-y).length(), w2 = Vector2f(rx-x,y-ly).length(), w3 = Vector2f(rx-x,ry-y).length();
        return (
            texture->get(lx%texture->w,ly%texture->h) * w0
        +   texture->get(lx%texture->w,ry%texture->h) * w1
        +   texture->get(rx%texture->w,ly%texture->h) * w2
        +   texture->get(rx%texture->w,ry%texture->h) * w3
        ) / (w0+w1+w2+w3);
    }
};

#endif // MATERIAL_H