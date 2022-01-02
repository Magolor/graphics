#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>


class Mesh : public Object3D {
public:
    struct TriangleIndex {
        TriangleIndex() {x[0] = 0; x[1] = 0; x[2] = 0;}
        int &operator[](const int i) { return x[i]; }
        int x[3]{};
    };
    Mesh(const char *filename, Material *material) : Object3D(material) {
        // Optional: Use tiny obj loader to replace this simple one.
        std::ifstream f;
        f.open(filename);
        if (!f.is_open()) {
            std::cout << "Cannot open " << filename << "\n";
            return;
        }
        std::string line;
        std::string vTok("v");
        std::string fTok("f");
        std::string texTok("vt");
        char bslash = '/', space = ' ';
        std::string tok;
        int texID;
        while (true) {
            std::getline(f, line);
            if (f.eof()) {
                break;
            }
            if (line.size() < 3) {
                continue;
            }
            if (line.at(0) == '#') {
                continue;
            }
            std::stringstream ss(line);
            ss >> tok;
            if (tok == vTok) {
                Vector3f vec;
                ss >> vec[0] >> vec[1] >> vec[2];
                v.push_back(vec);
            } else if (tok == fTok) {
                if (line.find(bslash) != std::string::npos) {
                    std::replace(line.begin(), line.end(), bslash, space);
                    std::stringstream facess(line);
                    TriangleIndex trig;
                    facess >> tok;
                    for (int ii = 0; ii < 3; ii++) {
                        facess >> trig[ii] >> texID;
                        trig[ii]--;
                    }
                    t.push_back(trig);
                } else {
                    TriangleIndex trig;
                    for (int ii = 0; ii < 3; ii++) {
                        ss >> trig[ii];
                        trig[ii]--;
                    }
                    t.push_back(trig);
                }
            } else if (tok == texTok) {
                Vector2f texcoord;
                ss >> texcoord[0];
                ss >> texcoord[1];
            }
        }
        computeNormal();
        f.close();
    }

    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    bool intersect(const Ray &r, Hit &h, float tmin) override;

private:
    void computeNormal(){
        n.resize(t.size());
        for (int triId = 0; triId < (int) t.size(); ++triId) {
            TriangleIndex& triIndex = t[triId];
            Vector3f a = v[triIndex[1]] - v[triIndex[0]];
            Vector3f b = v[triIndex[2]] - v[triIndex[0]];
            b = Vector3f::cross(a, b);
            n[triId] = b / b.length();
        }
    }
};

#endif
