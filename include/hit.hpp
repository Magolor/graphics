#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:
    float t;
    Material *material;
    Vector3f normal;

    Hit() : material(nullptr), t(1e38) {}
    Hit(float _t, Material *m, const Vector3f &n) : t(_t), material(m), normal(n.normalized()) {}
    Hit(const Hit &h) : t(h.t), material(h.material), normal(h.normal) {}
    ~Hit() = default;

    void set(float _t, Material *m, const Vector3f &n) {t = _t; material = m; normal = n.normalized();}
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.t << ", " << h.normal << ">"; return os;
}

#endif // HIT_H