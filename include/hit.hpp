#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Triangle;
class Material;

class Hit {
public:
    double t;
    bool has_texture;
    bool has_normcoord;
    Triangle *portal;
    Material *material;
    Vector3f normal;
    Vector2f texcoord;
    Vector3f normcoord;

    Hit() : material(nullptr), t(1e38), portal(nullptr), has_texture(false) {}
    Hit(double _t, Material *m, const Vector3f &n) : t(_t), material(m), normal(n.normalized()), texcoord(Vector2f::ZERO), normcoord(Vector3f::ZERO), portal(nullptr), has_texture(false) {}
    Hit(const Hit &h) : t(h.t), material(h.material), normal(h.normal), texcoord(h.texcoord), normcoord(h.normcoord), portal(nullptr), has_texture(h.has_texture) {}
    ~Hit() = default;

    void __set(double _t, Material *m, const Vector3f &n) {t = _t; material = m; normal = n.normalized();}
    void set(double _t, Material *m, const Vector3f &n) {__set(_t, m, n); portal = nullptr; has_texture = false; has_normcoord = false;}
    void set(double _t, Material *m, const Vector3f &n, const Vector2f &vt, bool un, const Vector3f &vn) {__set(_t, m, n); texcoord = vt; portal = nullptr; has_texture = true; has_normcoord = un; normcoord = vn;}
    void set(double _t, Material *m, const Vector3f &n, Triangle *a) {__set(_t, m, n); portal = a; has_texture = false; has_normcoord = false;}
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.t << ", " << h.normal << ">"; return os;
}

#endif // HIT_H