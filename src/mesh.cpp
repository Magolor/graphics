#include "mesh.hpp"
#define TINYOBJLOADER_USE_DOUBLE
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define MIN_KDTREE_BUILD_SIZE 32

Mesh::Mesh(const char *filename, Material *material) : Object3D(material), T(nullptr) {
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
    for(auto m:materials)
        if(M.count(m.name)==0){
            Vector4f albedo = material->albedo;
            // Just meaninglessly making use of parameters
            albedo[2] = (m.     emission[0]+m.     emission[1]+m.     emission[2])/3.;
            albedo[3] = (m.transmittance[0]+m.transmittance[1]+m.transmittance[2])/3.;
            M[m.name] = new Material(
                Vector3f(m. diffuse[0], m. diffuse[1], m. diffuse[2]),
                Vector3f(m.specular[0], m.specular[1], m.specular[2]),
                Vector3f(m. ambient[0], m. ambient[1], m. ambient[2]),
                m.shininess,
                m.ior,
                albedo,
                m.diffuse_texname.c_str()
            );
        }

    tinyobj::index_t idx[3];
    for(int si = 0, offset, i, j; si < shapes.size(); si++)
        for(int fi = offset = 0; fi < shapes[si].mesh.num_face_vertices.size(); offset += 3, fi++){
            assert(int(shapes[si].mesh.num_face_vertices[fi])==3);
            int mat = shapes[si].mesh.material_ids[fi];
            Triangle a(mat>=0?M[materials[mat].name]:material); a.un = a.ut = 1;
            for(i = 3; i--; idx[i] = shapes[si].mesh.indices[offset+i]);
            for(i = 3; i--; ) for(j = 3; j--; a.v[i][j] = attrib.vertices[3*idx[i].vertex_index+j]);
            for(i = 3; i--; ) if(idx[i].  normal_index>=0) for(j = 3; j--; a.vn[i][j] = attrib.  normals[3*idx[i].  normal_index+j]); else a.un = 0;
            for(i = 3; i--; ) if(idx[i].texcoord_index>=0) for(j = 2; j--; a.vt[i][j] = attrib.texcoords[2*idx[i].texcoord_index+j]); else a.ut = 0;
            a.normal = Vector3f::cross(a.v[1]-a.v[0],a.v[2]-a.v[0]).normalized(); A.push_back(a);
        }
    if(A.size()>=MIN_KDTREE_BUILD_SIZE) T = new KDTree(&A);
}

Mesh::Mesh(vector<Triangle> *triangles, Material *material) : Object3D(material), T(nullptr) {
    for(auto &triangle:*triangles) A.push_back(triangle);
    if(A.size()>=MIN_KDTREE_BUILD_SIZE) T = new KDTree(&A);
}

bool Mesh::intersect(const Ray &r, Hit &h, double tmin) {
    if(T==nullptr){bool f=false; for(auto o:A) f = o.intersect(r,h,tmin) || f; return f;} else return T->intersect(r,h,tmin);
}