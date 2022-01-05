#include "utils.hpp"
#include "scene_parser.hpp"
#include "tqdm.h"
#include <cmath>

#define MIN_RESOLUTION 64

bool SceneParser::shadowTracing(const Ray &r, const Hit &h, const Vector3f &dirToLight, double eps) const {
    Vector3f p = r.p(h.t); p = p+(Vector3f::dot(dirToLight,h.normal)<0.?-eps:eps)*h.normal; Hit h2; return group->intersect(Ray(p,dirToLight), h2, 0) && h2.t<dirToLight.length();
}

Vector3f SceneParser::rayTracing(const Ray &r, const Vector3f &background, int depth, double weight, double eps) const {
    Hit h = Hit(); if(weight < 1e-6) depth = 0; if( !group->intersect(r, h, 0) ){ if(texture==nullptr) return background;
        double theta = acosl(min(max(r.d[1]/r.d.length(),-1.),1.)), phi = atan2l(r.d[2],r.d[0])+M_PI;
        return texture->get(max(min(int(phi/2./M_PI*texture->w),texture->w-1),0), max(min(int(theta/M_PI*texture->h),texture->h-1),0));
    }
    Vector3f p = r.p(h.t), d, lc; Light *l;
    Vector3f Id = r.d, Nd = h.normal, Rd = reflect(Id,Nd), Fd = refract(Id,Nd,h.material->refractivity);
    Vector3f Ro = p+(Vector3f::dot(Rd,Nd)<0.?-eps:eps)*Nd, Fo = p+(Vector3f::dot(Fd,Nd)<0.?-eps:eps)*Nd;
    Vector3f reflection = depth>0&&h.material->albedo[2]>0.?rayTracing(Ray(Ro,Rd.normalized()),background,depth-1,weight*h.material->albedo[2],eps):Vector3f::ZERO;
    Vector3f refraction = depth>0&&h.material->albedo[3]>0.?rayTracing(Ray(Fo,Fd.normalized()),background,depth-1,weight*h.material->albedo[3],eps):Vector3f::ZERO;
    Vector3f c = reflection * h.material->albedo[2] + refraction * h.material->albedo[3];
    // if(h.portal!=nullptr && depth>0 && h.material->portalness>0.){
    //     Ray transmit(p,Id); Ray relative = h.portal->getPortalRelative(transmit); Ray port = h.portal->portal->getPortalRay(relative);
    //     Vector3f portation = rayTracing(port,background,depth-1,weight*h.material->portalness,eps); c += h.material->portalness*portation;
    // }
    for(int k = nLights; k-- && ((l=getLight(k))->getIllumination(p, d, lc), true); )
        c += l->getIntensity()*h.material->Shade(r, h, d, lc, shadowTracing(r, h, d)); return c;
}

void SceneParser::render(const char *filename, int depth, bool preview) const {
    Ray r; Vector3f c; tqdm bar; int K, W = camera->w, H = camera->h, G = W*H*4, T = 0;
    for(K = 1; !(W&1)&&!(H&1)&&W>=MIN_RESOLUTION&&H>=MIN_RESOLUTION; W >>= 1, H >>= 1, K++); Image *img; bool f=true;
    for(img = preview?new Image(W,H):(W<<=K,H<<=K,K=0,new Image(W,H)); K>=0; K?W<<=1,H<<=1,img=img->x2(),img->save(filename),0:0, f=false, K--)
        for(int x = 0; x < W; T += !f&&!(x&1)?H>>1:H, bar.progress(T, G), x++)
            if(!f&&!(x&1)){
                // #pragma omp parallel for num_threads(64)
                for(int y = 1; y < H; y += 2)
                    img->set(x,y,rayTracing(camera->generateRay(Vector2f((x<<K)/2.,(y<<K)/2.)),color,depth));
            } else {
                // #pragma omp parallel for num_threads(64)
                for(int y = 0; y < H; y++)
                    img->set(x,y,rayTracing(camera->generateRay(Vector2f((x<<K)/2.,(y<<K)/2.)),color,depth));
            }
    img->d2()->save(filename); bar.finish();
}