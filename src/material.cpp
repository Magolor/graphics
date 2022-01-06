#include "utils.hpp"
#include "material.hpp"

Vector3f Material::Shade(const Ray &ray, const Hit &hit, const Vector3f &dirToLight, const Vector3f &lightColor, bool shadow) {
    Vector3f shaded = Vector3f::ZERO, V = -ray.d.normalized(), L = dirToLight.normalized(), N = hit.normal, R = reflect(-L,N).normalized();
    Vector3f diffuse_texture = (!shadow ? hit.has_texture ? getNearestCyclicTexture(hit.texcoord) : diffuseColor : Vector3f::ZERO) + ambienceColor;
    Vector3f diffusion = diffuse_texture * max(0.,Vector3f::dot(L, N)), speculum = specularColor * min(pow(min(max(0.,Vector3f::dot(V, R)),1.),shininess), 1.);
    return lightColor * (diffusion * albedo[0] + speculum * albedo[1]);
}