#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

class Triangle: public Object3D {
public:
	Vector3f normal;
	Vector3f x[3];

	Triangle() = delete;
	Triangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m)
		 : Object3D(m), normal(Vector3f::cross(b-a,c-a).normalized()) {x[0] = a; x[1] = b; x[2] = c;}

	bool intersect(const Ray& l,  Hit& h, float tmin) override {
        Vector3f o = l.o, p = l.d, e1 = x[0]-x[1], e2 = x[0]-x[2], s = x[0]-o;
		float d = Matrix3f(p,e1,e2).determinant(), t = 0., b = 0., c = 0.;
        return (t=Matrix3f(s,e1,e2).determinant()/d)>=tmin&&t<h.t
			&& (b=Matrix3f(p,s,e2).determinant()/d)>=0&&b<=1
			&& (c=Matrix3f(p,e1,s).determinant()/d)>=0&&c<=1-b
			&& (h.set(t,material,normal),true);
	}
};

#endif //TRIANGLE_H
