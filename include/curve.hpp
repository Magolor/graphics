#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
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
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

    void drawGL() override {
        Object3D::drawGL();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);
        glBegin(GL_LINE_STRIP);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        glPointSize(4);
        glBegin(GL_POINTS);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        std::vector<CurvePoint> sampledPoints;
        discretize(30, sampledPoints);
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (auto & cp : sampledPoints) { glVertex3fv(cp.V); }
        glEnd();
        glPopAttrib();
    }
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear(); Bezier b(controls); double d = 1./resolution, t = 0.;
        for(int i = 0; i < resolution; data.push_back(CurvePoint{b.f(t),b.df(t)}), t += d, i++);
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear(); Bspline b(controls); double d = 1./(b.n+b.k)/resolution, t;
        for(int i = b.k, j; i <= b.n; i++) for(t = b.T[i], j = 0; j < resolution; b.deBoor(t), data.push_back(CurvePoint{b.f(t),b.df(t)}), t += d, j++);
    }

protected:

};

#endif // CURVE_HPP