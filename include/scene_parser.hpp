#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include <cassert>
#include <vecmath.h>

#include "camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include "object3d.hpp"
#include "group.hpp"
#include "mesh.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "transform.hpp"
#include "utils.hpp"
#include "image.hpp"

#define MAX_PARSER_TOKEN_LENGTH 1024

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

class SceneParser {
public:
    Group *group;
    Camera *camera;
    Image *texture;
    Vector3f color;
    int nLights, nMaterials;

    SceneParser() = delete;
    SceneParser(const char *filename) {
        // initialize some reasonable default values
        group = nullptr;
        camera = nullptr;
        texture = nullptr;
        color = Vector3f(0.5, 0.5, 0.5);
        nLights = 0;
        lights = nullptr;
        nMaterials = 0;
        materials = nullptr;
        current_material = nullptr;
    
        // parse the file
        assert(filename != nullptr);
        const char *ext = &filename[strlen(filename) - 4];
    
        if (strcmp(ext, ".txt") != 0) {
            printf("wrong file name extension\n");
            exit(0);
        }
        file = fopen(filename, "r");
    
        if (file == nullptr) {
            printf("cannot open scene file\n");
            exit(0);
        }
        parseFile();
        fclose(file);
        file = nullptr;
    
        if (nLights == 0) {
            printf("WARNING:    No lights specified\n");
        }
    }
    
    ~SceneParser() {
        delete group;
        delete camera;
        delete texture;
    
        int i;
        for (i = 0; i < nMaterials; i++) {
            delete materials[i];
        }
        delete[] materials;
        for (i = 0; i < nLights; i++) {
            delete lights[i];
        }
        delete[] lights;
    }

    Light **getLights() const {return lights;}
    Light *getLight(int i) const {assert(i>=0 && i<nLights); return lights[i];}
    Material *getMaterial(int i) const {assert(i>=0 && i<nMaterials); return materials[i];}
    bool shadowTracing(const Ray &r, const Hit &hit, const Vector3f &dirToLight, double eps = 1e-7) const;
    Vector3f rayTracing(const Ray &r, const Vector3f &background, int depth = 4, double eps = 1e-7) const;
    Image render(int depth = 4) const;

private:
    void parseFile() {
        //
        // at the top level, the scene can have a camera, 
        // background color and a group of objects
        // (we add lights and other things in future assignments)
        //
        char token[MAX_PARSER_TOKEN_LENGTH];
        while (getToken(token)) {
            if (!strcmp(token, "PerspectiveCamera")) {
                parsePerspectiveCamera();
            } else if (!strcmp(token, "Background")) {
                parseBackground();
            } else if (!strcmp(token, "Lights")) {
                parseLights();
            } else if (!strcmp(token, "Materials")) {
                parseMaterials();
            } else if (!strcmp(token, "Group")) {
                group = parseGroup();
            } else {
                printf("Unknown token in parseFile: '%s'\n", token);
                exit(0);
            }
        }
    }
    
    // ====================================================================
    // ====================================================================
    
    void parsePerspectiveCamera() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        // read in the camera parameters
        getToken(token);
        assert (!strcmp(token, "{"));
        getToken(token);
        assert (!strcmp(token, "center"));
        Vector3f center = readVector3f();
        getToken(token);
        assert (!strcmp(token, "direction"));
        Vector3f direction = readVector3f();
        getToken(token);
        assert (!strcmp(token, "up"));
        Vector3f up = readVector3f();
        getToken(token);
        assert (!strcmp(token, "angle"));
        double angle_degrees = readDouble();
        double angle_radians = DegreesToRadians(angle_degrees);
        getToken(token);
        assert (!strcmp(token, "width"));
        int width = readInt();
        getToken(token);
        assert (!strcmp(token, "height"));
        int height = readInt();
        getToken(token);
        assert (!strcmp(token, "}"));
        camera = new PerspectiveCamera(center, direction, up, width, height, angle_radians);
    }
    
    void parseBackground() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        char filename[MAX_PARSER_TOKEN_LENGTH];
        // read in the background color
        getToken(token);
        assert (!strcmp(token, "{"));
        while (true) {
            getToken(token);
            if (!strcmp(token, "}")) {
                break;
            } else if (!strcmp(token, "color")) {
                color = readVector3f();
            } else if (!strcmp(token, "texture")) {
                getToken(filename);
                texture = readTexture(filename);
            } else {
                printf("Unknown token in parseBackground: '%s'\n", token);
                assert(0);
            }
        }
    }
    
    // ====================================================================
    // ====================================================================
    
    void parseLights() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        getToken(token);
        assert (!strcmp(token, "{"));
        // read in the number of objects
        getToken(token);
        assert (!strcmp(token, "numLights"));
        nLights = readInt();
        lights = new Light *[nLights];
        // read in the objects
        int count = 0;
        while (nLights > count) {
            getToken(token);
            if (strcmp(token, "DirectionalLight") == 0) {
                lights[count] = parseDirectionalLight();
            } else if (strcmp(token, "PointLight") == 0) {
                lights[count] = parsePointLight();
            } else {
                printf("Unknown token in parseLight: '%s'\n", token);
                exit(0);
            }
            count++;
        }
        getToken(token);
        assert (!strcmp(token, "}"));
    }
    
    Light *parseDirectionalLight() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        double intensity = 1.0;
        getToken(token);
        assert (!strcmp(token, "{"));
        getToken(token);
        assert (!strcmp(token, "direction"));
        Vector3f direction = readVector3f();
        getToken(token);
        assert (!strcmp(token, "color"));
        Vector3f color = readVector3f();
        getToken(token);
        if(!strcmp(token, "intensity")){
            intensity = readDouble();
            getToken(token);
        }
        assert (!strcmp(token, "}"));
        return new DirectionalLight(direction, color, intensity);
    }
    
    Light *parsePointLight() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        double intensity = 1.0;
        getToken(token);
        assert (!strcmp(token, "{"));
        getToken(token);
        assert (!strcmp(token, "position"));
        Vector3f position = readVector3f();
        getToken(token);
        assert (!strcmp(token, "color"));
        Vector3f color = readVector3f();
        getToken(token);
        if(!strcmp(token, "intensity")){
            intensity = readDouble();
            getToken(token);
        }
        assert (!strcmp(token, "}"));
        return new PointLight(position, color, intensity);
    }
    // ====================================================================
    // ====================================================================
    
    void parseMaterials() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        getToken(token);
        assert (!strcmp(token, "{"));
        // read in the number of objects
        getToken(token);
        assert (!strcmp(token, "numMaterials"));
        nMaterials = readInt();
        materials = new Material *[nMaterials];
        // read in the objects
        int count = 0;
        while (nMaterials > count) {
            getToken(token);
            if (!strcmp(token, "Material") ||
                !strcmp(token, "PhongMaterial")) {
                materials[count] = parseMaterial();
            } else {
                printf("Unknown token in parseMaterial: '%s'\n", token);
                exit(0);
            }
            count++;
        }
        getToken(token);
        assert (!strcmp(token, "}"));
    }
    
    Material *parseMaterial() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        char filename[MAX_PARSER_TOKEN_LENGTH];
        filename[0] = 0;
        Vector4f albedo(1, 1, 0, 0);
        Vector3f diffuseColor(1, 1, 1), specularColor(0, 0, 0);
        double shininess = 0, refractivity = 1;
        getToken(token);
        assert (!strcmp(token, "{"));
        while (true) {
            getToken(token);
            if (strcmp(token, "diffuseColor") == 0) {
                diffuseColor = readVector3f();
            } else if (strcmp(token, "specularColor") == 0) {
                specularColor = readVector3f();
            } else if (strcmp(token, "shininess") == 0) {
                shininess = readDouble();
            } else if (strcmp(token, "refractivity") == 0) {
                refractivity = readDouble();
            } else if (strcmp(token, "albedo") == 0) {
                albedo = readVector4f();
            } else if (strcmp(token, "texture") == 0) {
                // Optional: read in texture and draw it.
                getToken(filename);
            } else {
                assert (!strcmp(token, "}"));
                break;
            }
        }
        auto *answer = new Material(diffuseColor, specularColor, shininess, refractivity, albedo);
        return answer;
    }
    
    // ====================================================================
    // ====================================================================
    
    Object3D *parseObject(char token[MAX_PARSER_TOKEN_LENGTH]) {
        Object3D *answer = nullptr;
        if (!strcmp(token, "Group")) {
            answer = (Object3D *) parseGroup();
        } else if (!strcmp(token, "Sphere")) {
            answer = (Object3D *) parseSphere();
        } else if (!strcmp(token, "Plane")) {
            answer = (Object3D *) parsePlane();
        } else if (!strcmp(token, "Triangle")) {
            answer = (Object3D *) parseTriangle();
        } else if (!strcmp(token, "TriangleMesh")) {
            answer = (Object3D *) parseTriangleMesh();
        } else if (!strcmp(token, "Transform")) {
            answer = (Object3D *) parseTransform();
        } else {
            printf("Unknown token in parseObject: '%s'\n", token);
            exit(0);
        }
        return answer;
    }
    
    // ====================================================================
    // ====================================================================
    
    Group *parseGroup() {
        //
        // each group starts with an integer that specifies
        // the number of objects in the group
        //
        // the material index sets the material of all objects which follow,
        // until the next material index (scoping for the materials is very
        // simple, and essentially ignores any tree hierarchy)
        //
        char token[MAX_PARSER_TOKEN_LENGTH];
        char filename[MAX_PARSER_TOKEN_LENGTH];
        filename[0] = 0;
        getToken(token);
        assert (!strcmp(token, "{"));
    
        // read in the number of objects
        getToken(token);
        assert (!strcmp(token, "numObjects"));
        int nObjects = readInt();
    
        auto *answer = new Group(nObjects);
    
        // read in the objects
        int count = 0;
        while (nObjects > count) {
            getToken(token);
            if (!strcmp(token, "MaterialIndex")) {
                // change the current material
                int index = readInt();
                assert (index >= 0 && index <= getNumMaterials());
                current_material = getMaterial(index);
            } else if (!strcmp(token, "MaterialPreset")) {
                // change the current material
                getToken(filename);
                current_material = new Material(filename);
            } else {
                Object3D *object = parseObject(token);
                assert (object != nullptr);
                answer->insert(count, object);
    
                count++;
            }
        }
        getToken(token);
        assert (!strcmp(token, "}"));
    
        // return the group
        return answer;
    }
    
    // ====================================================================
    // ====================================================================
    
    Sphere *parseSphere() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        getToken(token);
        assert (!strcmp(token, "{"));
        getToken(token);
        assert (!strcmp(token, "center"));
        Vector3f center = readVector3f();
        getToken(token);
        assert (!strcmp(token, "radius"));
        double radius = readDouble();
        getToken(token);
        assert (!strcmp(token, "}"));
        assert (current_material != nullptr);
        return new Sphere(center, radius, current_material);
    }
    
    Plane *parsePlane() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        getToken(token);
        assert (!strcmp(token, "{"));
        getToken(token);
        assert (!strcmp(token, "normal"));
        Vector3f normal = readVector3f();
        getToken(token);
        assert (!strcmp(token, "offset"));
        double offset = readDouble();
        getToken(token);
        assert (!strcmp(token, "}"));
        assert (current_material != nullptr);
        return new Plane(normal, offset, current_material);
    }
    
    Triangle *parseTriangle() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        getToken(token);
        assert (!strcmp(token, "{"));
        getToken(token);
        assert (!strcmp(token, "vertex0"));
        Vector3f v0 = readVector3f();
        getToken(token);
        assert (!strcmp(token, "vertex1"));
        Vector3f v1 = readVector3f();
        getToken(token);
        assert (!strcmp(token, "vertex2"));
        Vector3f v2 = readVector3f();
        getToken(token);
        assert (!strcmp(token, "}"));
        assert (current_material != nullptr);
        return new Triangle(v0, v1, v2, current_material);
    }
    
    Mesh *parseTriangleMesh() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        char filename[MAX_PARSER_TOKEN_LENGTH];
        // get the filename
        getToken(token);
        assert (!strcmp(token, "{"));
        getToken(token);
        assert (!strcmp(token, "obj_file"));
        getToken(filename);
        getToken(token);
        assert (!strcmp(token, "}"));
        const char *ext = &filename[strlen(filename) - 4];
        assert(!strcmp(ext, ".obj"));
        Mesh *answer = new Mesh(filename, current_material);
    
        return answer;
    }
    
    Transform *parseTransform() {
        char token[MAX_PARSER_TOKEN_LENGTH];
        Matrix4f matrix = Matrix4f::identity();
        Object3D *object = nullptr;
        getToken(token);
        assert (!strcmp(token, "{"));
        // read in transformations: 
        // apply to the LEFT side of the current matrix (so the first
        // transform in the list is the last applied to the object)
        getToken(token);
    
        while (true) {
            if (!strcmp(token, "Scale")) {
                Vector3f s = readVector3f();
                matrix = matrix * Matrix4f::scaling(s[0], s[1], s[2]);
            } else if (!strcmp(token, "UniformScale")) {
                double s = readDouble();
                matrix = matrix * Matrix4f::uniformScaling(s);
            } else if (!strcmp(token, "Translate")) {
                matrix = matrix * Matrix4f::translation(readVector3f());
            } else if (!strcmp(token, "XRotate")) {
                matrix = matrix * Matrix4f::rotateX(DegreesToRadians(readDouble()));
            } else if (!strcmp(token, "YRotate")) {
                matrix = matrix * Matrix4f::rotateY(DegreesToRadians(readDouble()));
            } else if (!strcmp(token, "ZRotate")) {
                matrix = matrix * Matrix4f::rotateZ(DegreesToRadians(readDouble()));
            } else if (!strcmp(token, "Rotate")) {
                getToken(token);
                assert (!strcmp(token, "{"));
                Vector3f axis = readVector3f();
                double degrees = readDouble();
                double radians = DegreesToRadians(degrees);
                matrix = matrix * Matrix4f::rotation(axis, radians);
                getToken(token);
                assert (!strcmp(token, "}"));
            } else if (!strcmp(token, "Matrix4f")) {
                Matrix4f matrix2 = Matrix4f::identity();
                getToken(token);
                assert (!strcmp(token, "{"));
                for (int j = 0; j < 4; j++) {
                    for (int i = 0; i < 4; i++) {
                        double v = readDouble();
                        matrix2(i, j) = v;
                    }
                }
                getToken(token);
                assert (!strcmp(token, "}"));
                matrix = matrix2 * matrix;
            } else {
                // otherwise this must be an object,
                // and there are no more transformations
                object = parseObject(token);
                break;
            }
            getToken(token);
        }
    
        assert(object != nullptr);
        getToken(token);
        assert (!strcmp(token, "}"));
        return new Transform(matrix, object);
    }
    
    // ====================================================================
    // ====================================================================
    
    int getToken(char token[MAX_PARSER_TOKEN_LENGTH]) {
        // for simplicity, tokens must be separated by whitespace
        assert (file != nullptr);
        int success = fscanf(file, "%s ", token);
        if (success == EOF) {
            token[0] = '\0';
            return 0;
        }
        return 1;
    }
    
    Image *readTexture(const char *filename) {
        return new Image(filename);
    }

    Vector4f readVector4f() {
        double x, y, z, w;
        int count = fscanf(file, "%lf %lf %lf %lf", &x, &y, &z, &w);
        if (count != 4) {
            printf("Error trying to read 4 floats to make a Vector3f\n");
            assert (0);
        }
        return Vector4f(x, y, z, w);
    }
    
    Vector3f readVector3f() {
        double x, y, z;
        int count = fscanf(file, "%lf %lf %lf", &x, &y, &z);
        if (count != 3) {
            printf("Error trying to read 3 floats to make a Vector3f\n");
            assert (0);
        }
        return Vector3f(x, y, z);
    }
    
    double readDouble() {
        double answer;
        int count = fscanf(file, "%lf", &answer);
        if (count != 1) {
            printf("Error trying to read 1 double\n");
            assert (0);
        }
        return answer;
    }
    
    int readInt() {
        int answer;
        int count = fscanf(file, "%d", &answer);
        if (count != 1) {
            printf("Error trying to read 1 int\n");
            assert (0);
        }
        return answer;
    }

    FILE *file;
    Light **lights;
    Material **materials;
    Material *current_material;
};

#endif // SCENE_PARSER_H
