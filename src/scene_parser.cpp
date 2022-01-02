#include "utils.hpp"
#include "scene_parser.hpp"
#include "tqdm.h"
#include <cmath>

bool SceneParser::shadowTracing(const Ray &r, const Hit &h, const Vector3f &dirToLight, double eps) const {
    Vector3f p = r.p(h.t); p = p+(Vector3f::dot(dirToLight,h.normal)<0.?-eps:eps)*h.normal; Hit h2; return group->intersect(Ray(p,dirToLight), h2, 0) && h2.t<dirToLight.length();
}

Vector3f SceneParser::rayTracing(const Ray &r, const Vector3f &background, int depth, double weight, double eps) const {
    Hit h = Hit(); if(weight < 1e-4) return Vector3f::ZERO; if( !group->intersect(r, h, 0) ){ if(texture->data==nullptr) return background;
        double theta = acosl(min(max(r.d[1]/r.d.length(),-1.),1.)), phi = atan2l(r.d[2],r.d[0])+M_PI;
        return texture->get(max(min(int(phi/2./M_PI*texture->w),texture->w-1),0), max(min(int(theta/M_PI*texture->h),texture->h-1),0));
    }
    Vector3f p = r.p(h.t), d, lc; Light *l;
    Vector3f Id = r.d, Nd = h.normal, Rd = reflect(Id,Nd), Fd = refract(Id,Nd,h.material->refractivity);
    Vector3f Ro = p+(Vector3f::dot(Rd,Nd)<0.?-eps:eps)*Nd, Fo = p+(Vector3f::dot(Fd,Nd)<0.?-eps:eps)*Nd;
    Vector3f reflection = depth>0&&h.material->albedo[2]>0.?rayTracing(Ray(Ro,Rd.normalized()),Vector3f::ZERO,depth-1,weight*h.material->albedo[2],eps):Vector3f::ZERO;
    Vector3f refraction = depth>0&&h.material->albedo[3]>0.?rayTracing(Ray(Fo,Fd.normalized()),Vector3f::ZERO,depth-1,weight*h.material->albedo[3],eps):Vector3f::ZERO;
    Vector3f c = reflection * h.material->albedo[2] + refraction * h.material->albedo[3];
    for(int k = nLights; k-- && ((l=getLight(k))->getIllumination(p, d, lc), true); )
        c += !shadowTracing(r, h, d) ? l->getIntensity()*h.material->Shade(r, h, d, lc) : Vector3f::ZERO; return c;
}

Image SceneParser::render(int depth) const {
    Image img(camera->w, camera->h); Ray r; Vector3f c; tqdm bar;
    #pragma omp parallel for
    for(register int x = 0, y; x < camera->w; bar.progress(x, camera->w), x++)
        for(y = 0; y < camera->h; y++)
            img.set(x,y,rayTracing(camera->generateRay(Vector2f(x,y)), color, depth));
    bar.finish(); return img;
}