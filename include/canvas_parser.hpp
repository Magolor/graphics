#pragma once

#include <vecmath.h>
#include <cassert>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "element.hpp"

#define MAX_PARSER_TOKEN_LENGTH 1024

class CanvasParser {
public:
    int w, h;

    CanvasParser() = delete;
    explicit CanvasParser(const char *filename);
    ~CanvasParser();

    int size() const {return elements.size();}
    Element *get(int i) const {assert(i>=0 && i<elements.size()); return elements[i];}

private:
    void parseFile();
    Element* parseLine();
    Element* parseCircle();
    Element* parseFill();

    int getToken(char token[MAX_PARSER_TOKEN_LENGTH]);

    Vector3f readVector3f();
    float readFloat();
    int readInt();

    FILE *file;
    std::vector<Element*> elements;
};

CanvasParser::CanvasParser(const char *filename) {
    w = h = 0;

    // parse the file
    assert(filename != nullptr);
    const char *ext = &filename[strlen(filename) - 4];

    if (strcmp(ext, ".txt") != 0) {
        printf("wrong file name extension\n");
        exit(0);
    }
    file = fopen(filename, "r");

    if (file == nullptr) {
        printf("cannot open canvas file\n");
        exit(0);
    }
    parseFile();
    fclose(file);
    file = nullptr;

    if (elements.empty()) {
        printf("WARNING:    No elements specified\n");
    }
}

CanvasParser::~CanvasParser() {
    for (auto & element : elements) {
        delete element;
    }
}

void CanvasParser::parseFile() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    // First two numbers are canvas size.
    w = readInt();
    h = readInt();

    Element* newElement = nullptr;
    while (getToken(token)) {
        if (!strcmp(token, "Line")) {
            newElement = parseLine();
        } else if (!strcmp(token, "Circle")) {
            newElement = parseCircle();
        } else if (!strcmp(token, "Fill")) {
            newElement = parseFill();
        } else {
            printf("Unknown token in parseFile: '%s'\n", token);
            exit(0);
        }
        elements.push_back(newElement);
    }
}

Element *CanvasParser::parseLine() {
    auto* l = new Line;
    l->xA = readInt(); l->yA = readInt();
    l->xB = readInt(); l->yB = readInt();
    l->color = readVector3f();
    return l;
}

Element *CanvasParser::parseCircle() {
    auto* circ = new Circle;
    circ->cx = readInt(); circ->cy = readInt();
    circ->radius = readInt();
    circ->color = readVector3f();
    return circ;
}

Element *CanvasParser::parseFill() {
    auto* fill = new Fill;
    fill->cx = readInt(); fill->cy = readInt();
    fill->color = readVector3f();
    return fill;
}

int CanvasParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH]) {
    // for simplicity, tokens must be separated by whitespace
    assert (file != nullptr);
    int success = fscanf(file, "%s ", token);
    if (success == EOF) {
        token[0] = '\0';
        return 0;
    }
    return 1;
}

Vector3f CanvasParser::readVector3f() {
    float x, y, z;
    int count = fscanf(file, "%f %f %f", &x, &y, &z);
    if (count != 3) {
        printf("Error trying to read 3 floats to make a Vector3f\n");
        assert (0);
    }
    return Vector3f(x, y, z);
}

float CanvasParser::readFloat() {
    float answer;
    int count = fscanf(file, "%f", &answer);
    if (count != 1) {
        printf("Error trying to read 1 float\n");
        assert (0);
    }
    return answer;
}

int CanvasParser::readInt() {
    int answer;
    int count = fscanf(file, "%d", &answer);
    if (count != 1) {
        printf("Error trying to read 1 int\n");
        assert (0);
    }
    return answer;
}