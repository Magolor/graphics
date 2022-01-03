#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:
    double t; bool has_texture;
    Material *material;
    Vector3f normal;
    Vector2f texcoord;

    Hit() : material(nullptr), t(1e38), has_texture(false) {}
    Hit(double _t, Material *m, const Vector3f &n) : t(_t), material(m), normal(n.normalized()), texcoord(Vector2f::ZERO), has_texture(false) {}
    Hit(const Hit &h) : t(h.t), material(h.material), normal(h.normal), texcoord(h.texcoord), has_texture(h.has_texture) {}
    ~Hit() = default;

    void set(double _t, Material *m, const Vector3f &n) {t = _t; material = m; normal = n.normalized(); has_texture = false;}
    void set(double _t, Material *m, const Vector3f &n, const Vector2f &vt) {t = _t; material = m; normal = n.normalized(); texcoord = vt; has_texture = true;}
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.t << ", " << h.normal << ">"; return os;
}

#endif // HIT_H