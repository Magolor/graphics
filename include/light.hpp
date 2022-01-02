#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include "object3d.hpp"

class Light {
public:
    double intensity;

    Light() = default;
    virtual ~Light() = default;

    virtual double getIntensity() const = 0;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const = 0;
};

class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;
    DirectionalLight(const Vector3f &d, const Vector3f &c, double intensity = 1.0) : d(d.normalized()), color(c), intensity(intensity) {}
    ~DirectionalLight() override = default;

    double getIntensity() const override {return intensity;}

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -d; col = color;
    }

private:
    double intensity;
    Vector3f d;
    Vector3f color;
};

class PointLight : public Light {
public:
    PointLight() = delete;
    PointLight(const Vector3f &p, const Vector3f &c, double intensity = 1.0) : o(p), color(c), intensity(intensity) {}
    ~PointLight() override = default;

    double getIntensity() const override {return intensity;}

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (o - p); dir = dir / dir.length(); col = color;
    }

private:
    double intensity;
    Vector3f o;
    Vector3f color;
};

#endif // LIGHT_H
