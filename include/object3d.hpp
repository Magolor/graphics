#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"

// Base class for all 3d entities.
class Object3D {
public:
    Object3D() : material(nullptr) {}
    virtual ~Object3D() = default;
    explicit Object3D(Material *material) : material(material) {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

protected:
    Material *material;
};

#endif