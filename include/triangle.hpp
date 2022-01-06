#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

class Triangle: public Object3D {
public:
	Vector3f *v, normal;
	Vector3f *vn; bool un;
	Vector2f *vt; bool ut;
	Triangle *portal;

	Triangle() = delete;
	Triangle(Material* m) : Object3D(m), un(false), ut(false), portal(nullptr) {v = new Vector3f[3]; vn = new Vector3f[3]; vt = new Vector2f[3];}
	Triangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m)
		 : Object3D(m), normal(Vector3f::cross(b-a,c-a).normalized()), un(false), ut(false), portal(nullptr) {v = new Vector3f[3]; vn = new Vector3f[3]; vt = new Vector2f[3]; v[0] = a; v[1] = b; v[2] = c;}

	bool intersect(const Ray& l,  Hit& h, double tmin) override {
        Vector3f o = l.o, p = l.d, e1 = v[0]-v[1], e2 = v[0]-v[2], s = v[0]-o;
		double d = Matrix3f(p,e1,e2).determinant(), t = 0., b = 0., c = 0.;
        return ((t=Matrix3f(s,e1,e2).determinant()/d)>=tmin&&t<h.t
			&& (b=Matrix3f(p,s,e2).determinant()/d)>=0&&b<=1
			&& (c=Matrix3f(p,e1,s).determinant()/d)>=0&&c<=1-b
			&& ((portal==nullptr)?ut?h.set(t,material,normal,getTexcoord(l.p(t))):h.set(t,material,normal):h.set(t,material,normal,this),true));
	}

	Vector2f getTexcoord(Vector3f p) const {
		double w0 = Vector3f::cross(p-v[1],p-v[2]).length();
		double w1 = Vector3f::cross(p-v[0],p-v[2]).length();
		double w2 = Vector3f::cross(p-v[0],p-v[1]).length();
		return (vt[0]*w0 + vt[1]*w1 + vt[2]*w2) / (w0+w1+w2);
	}

	Ray getPortalRelative(const Ray &l) const {
		double w0 = Vector3f::cross(l.o-v[1],l.o-v[2]).length();
		double w1 = Vector3f::cross(l.o-v[0],l.o-v[2]).length();
		double w2 = Vector3f::cross(l.o-v[0],l.o-v[1]).length();
		Vector3f bar = Vector3f(w0,w1,w2) / (w0+w1+w2), d = l.d.normalized();
		Vector3f axis0 = normal, axis1 = (v[1]-v[0]).normalized(), axis2 = Vector3f::cross(axis0,axis1);
		double a0 = Vector3f::dot(d,axis0);
		double a1 = Vector3f::dot(d,axis1);
		double a2 = Vector3f::dot(d,axis2);
		Vector3f dir = Vector3f(a0,a1,a2);
		return Ray(bar, dir);
	}

	Ray getPortalRay(const Ray &l) const {
		Vector3f axis0 = normal, axis1 = (v[1]-v[0]).normalized(), axis2 = Vector3f::cross(axis0,axis1);
		return Ray(l.o[0]*v[0]+l.o[1]*v[1]+l.o[2]*v[2], (l.d[0]*axis0+l.d[1]*axis1+l.d[2]*axis2).normalized());
	}
};

#endif //TRIANGLE_H
