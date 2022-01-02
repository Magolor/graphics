#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <iostream>
#include <ctime>
#include "scene_parser.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    int start = clock();
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: ./bin/FINAL <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    SceneParser scene(inputFile.c_str());
    scene.render(4).save(outputFile.c_str());
    int end = clock();
    printf("Total time elapsed: %.2fs.\n",(1.*(end-start)/CLOCKS_PER_SEC));
    return 0;
}

