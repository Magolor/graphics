#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include "triangle.hpp"
#include "material.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <cmath>
#include <algorithm>

class Bernstein {
public:
    int n, k; std::vector<Vector3f> P;
    explicit Bernstein(std::vector<Vector3f> P, int k=3) : P(P), n(P.size()), k(k) {}
    virtual double b(int i, int k, double t) = 0;
    virtual double db(int i, int k, double t) = 0;
    Vector3f  f(double t){Vector3f p; for(int i = 0; i < n; p += b(i,k,t)*P[i], i++); return p;}
    Vector3f df(double t){Vector3f p; for(int i = 0; i < n; p += db(i,k,t)*P[i], i++); return p;}
};

class Bezier : public Bernstein {
public:
    std::vector< std::vector<int> > C;
    explicit Bezier(std::vector<Vector3f> P) : Bernstein(P) {
        k = n-1; C.clear(); for(int i = 0; i < n; C.push_back(std::vector<int>(n)), C[i][i] = 1, C[i++][0] = 1);
        for(int i = 1, j; i < n; i++) for(j = 1; j <= i>>1; C[i][j] = C[i-1][j]+C[i-1][j-1], C[i][i-j] = C[i][j], j++);
    }
    double b(int i, int k, double t) override {return i>=0?C[k][i]*pow(1-t,n-1-i)*pow(t,i):0;}
    double db(int i, int k, double t) override {return k*(b(i-1,k-1,t)-b(i,k-1,t));}
};

class Bspline : public Bernstein {
public:
    std::vector< std::vector<double> > B; std::vector<double> T;
    explicit Bspline(std::vector<Vector3f> P, int k=3) : Bernstein(P) {T.clear(); for(int i = 0; i <= n+k; T.push_back(i++/(n+k+0.)));}
    void deBoor(double t) {
        B.clear(); for(int i = 0; i <= k; B.push_back(std::vector<double>(n+k+1)), i++); int j = std::upper_bound(T.begin(),T.end(),t)-T.begin()-1; B[0][j] = 1;
        for(int p = 1, i; p <= k; p++) for(i = max(0,j-p); i <= min(j,n+k-p); B[p][i] = (t-T[i])/(T[i+p]-T[i])*B[p-1][i]+(T[i+p+1]-t)/(T[i+p+1]-T[i+1])*B[p-1][i+1], i++);
    }
    double b(int i, int k, double t) override {return B[k][i];}
    double db(int i, int k, double t) override {return k*(b(i,k-1,t)/(T[i+k]-T[i])-b(i+1,k-1,t)/(T[i+k+1]-T[i+1]));}
};

struct CurvePoint {
    double t;
    Vector3f V;
    Vector3f T;
};

class Curve : public Object3D {
public:
    std::vector<Vector3f> controls;

    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    std::vector<Vector3f> &getControls() {return controls;}
    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;
    bool intersect(const Ray &r, Hit &h, double tmin) override {return false;}
    virtual Vector3f  f(double t){return Vector3f::ZERO;};
    virtual Vector3f df(double t){return Vector3f::ZERO;};
};

class BezierCurve : public Curve {
public:
    Bezier *b;
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear(); b = new Bezier(controls); double d = 1./resolution, t = 0.;
        for(int i = 0; i < resolution; data.push_back(CurvePoint{t,b->f(t),b->df(t)}), t += d, i++);
    }
    Vector3f  f(double t) override {return b->f(t);}
    Vector3f df(double t) override {return b->df(t);}
};

class BsplineCurve : public Curve {
public:
    Bspline *b;
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear(); b = new Bspline(controls); double d = 1./(b->n+b->k)/resolution, t;
        for(int i = b->k, j; i <= b->n; i++) for(t = b->T[i], j = 0; j < resolution; b->deBoor(t), data.push_back(CurvePoint{t,b->f(t),b->df(t)}), t += d, j++);
    }
    Vector3f  f(double t) override {return b->deBoor(t), b->f(t);}
    Vector3f df(double t) override {return b->deBoor(t), b->df(t);}
};

typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;
struct Surface {
    std::vector<Vector3f> VV;
    std::vector<Vector3f> VN;
    std::vector<Tup3u> VF;
};
class RevSurface : public Object3D {
public:
    Curve *pCurve;
    Surface surface;
    std::vector<Triangle> triangles;
    std::vector<CurvePoint> curvePoints;
    Mesh *D;
    int curve_resolution = 128;
    int angle_resolution = 512;
    int gn_iteraiton_steps = 6;
    double learning_rate = 0.01;
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        pCurve->discretize(curve_resolution, curvePoints);
        int steps = angle_resolution;
        for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {
            const CurvePoint &cp = curvePoints[ci];
            for (unsigned int i = 0; i < steps; ++i) {
                float t = (float) i / steps;
                Quat4f rot;
                rot.setAxisAngle(t * 2 * M_PI, Vector3f::UP);
                Vector3f pnew = Matrix3f::rotation(rot) * cp.V;
                Vector3f pNormal = Vector3f::cross(cp.T, -Vector3f::FORWARD);
                Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
                surface.VV.push_back(pnew+nnew*1e-3);
                surface.VN.push_back(nnew);
                int i1 = (i + 1 == steps) ? 0 : i + 1;
                if (ci != curvePoints.size() - 1) {
                    surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i);
                    surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1);
                }
            }
        }
        for(auto &f:surface.VF) triangles.push_back(Triangle(
            surface.VV[std::get<0>(f)], surface.VV[std::get<1>(f)], surface.VV[std::get<2>(f)], material
        ));
        for(int i = 0; i < triangles.size(); triangles[i].id = i, triangles[i].portal = &triangles[i], i++);
        D = new Mesh(&triangles, material);
    }

    ~RevSurface() override {delete pCurve; delete D;}

    bool intersect(const Ray &r, Hit &h, double tmin) {
        Hit _h; if(!D->intersect(r, _h, tmin)) return false; Vector3f dC_dt = r.d, C, S, P, T, N; Quat4f rot; Matrix3f R;
        double t = _h.t; int ID = std::get<0>(surface.VF[_h.portal->id]), _t = t;
        double u = curvePoints[ID/angle_resolution].t, _u = u;
        double v = (ID%angle_resolution)*2.*M_PI/angle_resolution, _v = v;
        double last_distance = 1e38;
        for(int i = 0; i <= gn_iteraiton_steps; i++){
            rot.setAxisAngle(v, Vector3f::UP); R = Matrix3f::rotation(rot); C = r.p(t); P = pCurve->f(u); S = R*P;
            if( (C-S).length()>last_distance ) break; last_distance = (C-S).length(); t = _t; u = _u; v = _v;
            T = pCurve->df(u); N = Vector3f::cross(T, -Vector3f::FORWARD); if(i == gn_iteraiton_steps) break;
            Vector3f dS_du = R*T, dS_dv = Vector3f(-sin(v)*P[0],0.,-cos(v)*P[0]);
            double K = min(1./Vector3f::dot(dC_dt,Vector3f::cross(dS_du,dS_dv)),2.); Vector3f df = C-S;
            _t = t - learning_rate * (Vector3f::dot(dS_du,Vector3f::cross(dS_dv,df))) / K;
            _u = u - learning_rate * (Vector3f::dot(dC_dt,Vector3f::cross(dS_dv,df))) / K;
            _v = v + learning_rate * (Vector3f::dot(dC_dt,Vector3f::cross(dS_du,df))) / K;
        }
        return t>=tmin&&t<h.t ? h.set(t,material,N), true : false;
    }
};

#endif // CURVE_HPP