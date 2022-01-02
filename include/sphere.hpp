#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Sphere : public Object3D {
public:
    Vector3f c;
    double r;
    
    Sphere() : Object3D(nullptr), c(Vector3f(0.,0.,0.)), r(1.) {}
    Sphere(const Vector3f &center, double radius, Material *material) : Object3D(material), c(center), r(radius) {}
    ~Sphere() override = default;

    bool intersect(const Ray &l, Hit &h, double tmin) override {
        Vector3f p = l.d.normalized(); double d2 = (c-l.o).squaredLength(), r2 = r*r, tp = Vector3f::dot(c-l.o,p), z2 = d2-tp*tp, t = 0.;
        return z2<r2&&(t=tp-(r2<d2?sqrt(r2-z2):-sqrt(r2-z2)))>=tmin&&t<h.t&&(h.set(t, material, l.p(t)-c), true);
    }
};

#endif