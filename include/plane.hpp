#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Plane : public Object3D {
public:
    Vector3f normal;
    float d;

    Plane() : Object3D(nullptr), normal(Vector3f(1.,0.,0.)), d(0.) {}
    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m), normal(normal.normalized()), d(-d) {}
    ~Plane() override = default;

    bool intersect(const Ray &l, Hit &h, float tmin) override {
        float t = 0.; return (t=-(d+Vector3f::dot(normal,l.o))/Vector3f::dot(normal,l.d))>=tmin&&t<h.t&&(h.set(t,material,normal),true);
    }
};

#endif //PLANE_H
		

