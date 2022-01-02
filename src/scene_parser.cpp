#include "utils.hpp"
#include "scene_parser.hpp"

bool SceneParser::shadowTracing(const Ray &r, const Hit &h, const Vector3f &dirToLight, float eps) const {
    Vector3f p = r.p(h.t); p = p+(Vector3f::dot(dirToLight,h.normal)<0.?-eps:eps)*h.normal; Hit h2; return group->intersect(Ray(p,dirToLight), h2,0);
}

Vector3f SceneParser::rayTracing(const Ray &r, const Vector3f &background, int depth, float eps) const {
    Hit h = Hit(); if( !group->intersect(r, h, 0) ) return background; Vector3f p = r.p(h.t), d, lc;
    Vector3f Id = r.d, Nd = h.normal, Rd = reflect(Id,Nd), Fd = refract(Id,Nd,h.material->refractivity);
    Vector3f Ro = p+(Vector3f::dot(Rd,Nd)<0.?-eps:eps)*Nd, Fo = p+(Vector3f::dot(Fd,Nd)<0.?-eps:eps)*Nd;
    Vector3f reflection = depth>0&&h.material->albedo[2]>0.?rayTracing(Ray(Ro,Rd.normalized()),Vector3f::ZERO,depth-1,eps):Vector3f::ZERO;
    Vector3f refraction = depth>0&&h.material->albedo[3]>0.?rayTracing(Ray(Fo,Fd.normalized()),Vector3f::ZERO,depth-1,eps):Vector3f::ZERO;
    Vector3f c = reflection * h.material->albedo[2] + refraction * h.material->albedo[3];
    for(int k = nLights; k-- && (getLight(k)->getIllumination(p, d, lc), true); )
        c += !shadowTracing(r, h, d) ? h.material->Shade(r, h, d, lc) : Vector3f::ZERO; return c;
}

Image SceneParser::render(int depth) const {
    Image img(camera->w, camera->h); Ray r; Vector3f c;
    for(int x = 0, y, k; x < camera->w; x++) for(y = 0; y < camera->h; y++)
        img.set(x,y,rayTracing(camera->generateRay(Vector2f(x,y)), color, depth));
    return img;
}