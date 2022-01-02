#include "utils.hpp"
#include "material.hpp"

Vector3f Material::Shade(const Ray &ray, const Hit &hit, const Vector3f &dirToLight, const Vector3f &lightColor) {
    Vector3f shaded = Vector3f::ZERO, V = -ray.d.normalized(), L = dirToLight.normalized(), N = hit.normal, R = reflect(-L,N);
    Vector3f diffusion = diffuseColor * max(double(0.),Vector3f::dot(L, N)), speculum = specularColor * pow(max(double(0.),Vector3f::dot(V, R)),shininess);
    return lightColor * (diffusion * albedo[0] + speculum * albedo[1]);
}