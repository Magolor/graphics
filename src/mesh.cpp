#include "mesh.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./";
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filename, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }
    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    tinyobj::index_t idx[3];
    for(int si = 0, offset = 0, i, j; si < shapes.size(); si++)
        for(int fi = 0; fi < shapes[si].mesh.num_face_vertices.size(); offset += 3, fi++){
            assert(int(shapes[si].mesh.num_face_vertices[fi])==3); Triangle a(material);
            for(i = 3; i--; idx[i] = shapes[si].mesh.indices[offset+i]);
            for(i = 3; i--; ) for(j = 3; j--; a.v[i][j] = attrib.vertices[3*idx[i].vertex_index+j]);
            for(i = 3; i--; ) if(a.un=idx[i].  normal_index>=0) for(j = 3; j--; a.vn[i][j] = attrib.  normals[3*idx[i].  normal_index+j]);
            for(i = 3; i--; ) if(a.ut=idx[i].texcoord_index>=0) for(j = 2; j--; a.vt[i][j] = attrib.texcoords[3*idx[i].texcoord_index+j]);
            a.normal = Vector3f::cross(a.v[1]-a.v[0],a.v[2]-a.v[0]).normalized(); A.push_back(a);
        }
}

bool Mesh::intersect(const Ray &r, Hit &h, double tmin) {
    // Optional: Change this brute force method into a faster one.
    bool f=false; for(auto o:A) f = o.intersect(r,h,tmin) || f; return f;
}