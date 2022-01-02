#include <iostream>
#include <cassert>
#include "image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

Image::Image(const char *filename){
    int bpp = 0; uint8_t* rgb_image = stbi_load(filename, &w, &h, &bpp, 3); assert(bpp==3);
    data = new Vector3f[w*h]; for(int i = 0; i < w*h*3; data[i/3][i%3] = int(rgb_image[i])/255., i++); stbi_image_free(rgb_image);
}