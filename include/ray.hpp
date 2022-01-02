#ifndef RAY_H
#define RAY_H

#include <cassert>
#include <iostream>
#include <Vector3f.h>

class Ray {
public:
    Vector3f o;
    Vector3f d;

    Ray() : o(Vector3f::ZERO), d(Vector3f::ZERO) {}
    Ray(const Vector3f &orig, const Vector3f &dir) : o(orig), d(dir) {}
    Ray(const Ray &r) : o(r.o), d(r.d) {}

    Vector3f p(double t) const {return o + d * t;}
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "Ray <" << r.o << ", " << r.d << ">"; return os;
}

#endif // RAY_H