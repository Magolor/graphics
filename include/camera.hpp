#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera {
public:
    int w, h;
    
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH)
        : center(center), direction(direction.normalized()), w(imgW), h(imgH) {
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
    }
    virtual ~Camera() = default;

    virtual Ray generateRay(const Vector2f &point) = 0;

protected:
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH, float angle)
         : Camera(center, direction, up, imgW, imgH), focal(h/2/tan(angle/2)) {}

    Ray generateRay(const Vector2f &point) override {
        return Ray(center, (Matrix3f(horizontal, -up, direction)*Vector3f(point[0]-w/2,h/2-point[1],focal)).normalized());
    }

    float focal;
};

#endif //CAMERA_H
