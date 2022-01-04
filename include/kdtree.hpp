#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <algorithm>
#include "object3d.hpp"
#include "triangle.hpp"

class BBox {
public:
    Vector3f l, r;
    BBox(){l = Vector3f(1e38,1e38,1e38), r = Vector3f(-1e38,-1e38,-1e38);}
    BBox(const BBox &b) : l(b.l), r(b.r) {}
    BBox(const Vector3f &l, const Vector3f &r) : l(l), r(r) {}
    BBox(const Triangle &a){
        for(int i = 3; i--; l[i] = std::min(std::min(a.v[0][i],a.v[1][i]),a.v[2][i])); l -= 1e-9;
        for(int i = 3; i--; r[i] = std::max(std::max(a.v[0][i],a.v[1][i]),a.v[2][i])); r += 1e-9;
    }
    BBox(const BBox &b1, const BBox &b2, const Triangle &a){
        for(int i = 3; i--; l[i] = std::min(std::min(a.v[0][i],a.v[1][i]),a.v[2][i])); l -= 1e-9;
        for(int i = 3; i--; r[i] = std::max(std::max(a.v[0][i],a.v[1][i]),a.v[2][i])); r += 1e-9;
        for(int i = 3; i--; l[i] = std::min(std::min(b1.l[i],b2.l[i]),l[i]));
        for(int i = 3; i--; r[i] = std::max(std::max(b1.r[i],b2.r[i]),r[i]));
    }
    bool slab(const Ray &k) const {
        if((l[0]>r[0])||(l[1]>r[1])||(l[2]>r[2])) return false; Vector3f tmin, tmax; double tmp;
        for(int i = 3; i--; tmin[i]>tmax[i]?tmp=tmin[i],tmin[i]=tmax[i],tmax[i]=tmp,0:0)
            tmin[i] = (l[i]-k.o[i])/k.d[i], tmax[i] = (r[i]-k.o[i])/k.d[i];
        return std::max(std::max(tmin[0],tmin[1]),tmin[2])<std::min(std::min(tmax[0],tmax[1]),tmax[2]);
    }
};

class KDNode {
public:
    int d; Triangle *a; BBox b; KDNode *l, *r;
    KDNode() : d(0), a(nullptr), l(nullptr), r(nullptr){}
    KDNode(Triangle *a) : d(0), a(a), b(*a), l(nullptr), r(nullptr) {}
};

class KDTree {
public:
    int n; std::vector<Triangle> *A; std::vector<KDNode*> T; KDNode *ROOT;
    KDTree() : A(nullptr), ROOT(nullptr) {}
    KDTree(std::vector<Triangle> *A) : A(A), ROOT(nullptr) {n = A->size(); for(auto &t:*A) T.push_back(new KDNode(&t)); ROOT = BuildKDTree(0, 0, n);}
    KDNode* BuildKDTree(int d, int L, int R) {
        if(L>=R) return nullptr; int M = (L+R)>>1; if(L==R-1){T[M]->d = d; return T[M];}
        nth_element(T.begin()+L,T.begin()+M,T.begin()+R,[d](KDNode*a, KDNode*b){return a->b.r[d%3]<b->b.r[d%3];}); 
        T[M]->l = BuildKDTree(d+1,L,M); T[M]->r = BuildKDTree(d+1,M+1,R);
        T[M]->b = BBox(T[M]->l==nullptr?BBox():T[M]->l->b,T[M]->r==nullptr?BBox():T[M]->r->b,*(T[M]->a)); T[M]->d = d; return T[M];
    }
    bool intersect(const Ray &r, Hit &h, double tmin) {
        return __intersect(r, h, tmin, ROOT);
    }
    bool __intersect(const Ray &r, Hit &h, double tmin, KDNode *p) {
        if((p==nullptr)||!p->b.slab(r)) return false; bool f = p->a->intersect(r, h, tmin);
        f = __intersect(r, h, tmin, p->l) || f; f = __intersect(r, h, tmin, p->r) || f; return f;
    }
};

#endif