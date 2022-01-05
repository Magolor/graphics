#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
    return (mat * Vector4f(point, 1)).xyz();
}

static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
    return (mat * Vector4f(dir, 0)).xyz();
}

class Transform : public Object3D {
public:
    Transform() {}
    Transform(const Matrix4f &m, Object3D *obj) : o(obj), T(m.inverse()) {}
    ~Transform() {}

    virtual bool intersect(const Ray &r, Hit &h, double tmin) {
        Vector3f trSource = transformPoint(T, r.o);
        Vector3f trDirection = transformDirection(T, r.d);
        Ray tr(trSource, trDirection); bool inter = o->intersect(tr, h, tmin);
        return inter?h.__set(h.t, h.material, transformDirection(T.transposed(), h.normal)),true:false;
    }

protected:
    Object3D *o;
    Matrix4f T;
};

#endif //TRANSFORM_H
