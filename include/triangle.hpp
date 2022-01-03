#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

class Triangle: public Object3D {
public:
	Vector3f v[3], normal;
	Vector3f vn[3]; bool un;
	Vector2f vt[3]; bool ut;

	Triangle() = delete;
	Triangle(Material* m) : Object3D(m), un(false), ut(false) {}
	Triangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m)
		 : Object3D(m), normal(Vector3f::cross(b-a,c-a).normalized()), un(false), ut(false) {v[0] = a; v[1] = b; v[2] = c;}

	bool intersect(const Ray& l,  Hit& h, double tmin) override {
        Vector3f o = l.o, p = l.d, e1 = v[0]-v[1], e2 = v[0]-v[2], s = v[0]-o;
		double d = Matrix3f(p,e1,e2).determinant(), t = 0., b = 0., c = 0.;
        return (t=Matrix3f(s,e1,e2).determinant()/d)>=tmin&&t<h.t
			&& (b=Matrix3f(p,s,e2).determinant()/d)>=0&&b<=1
			&& (c=Matrix3f(p,e1,s).determinant()/d)>=0&&c<=1-b
			&& (h.set(t,material,normal),true);
	}
};

#endif //TRIANGLE_H
