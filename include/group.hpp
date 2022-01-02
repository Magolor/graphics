#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>
using namespace std;

class Group : public Object3D {
public:
    vector<Object3D*> objs; 

    Group() {}
    explicit Group (int num_objects) {objs.reserve(num_objects);}
    ~Group() override {vector<Object3D*>().swap(objs);}

    int size() {return objs.size();}
    void insert(int index, Object3D *obj) {objs.insert(objs.begin()+index,obj);}
    bool intersect(const Ray &l, Hit &h, float tmin) override {bool f=false; for(auto o:objs) f = o->intersect(l,h,tmin) || f; return f;}
};

#endif
	
