#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vecmath.h>

#define sqr(x) ((x)*(x))

Vector3f reflect(const Vector3f &I, const Vector3f &N);
Vector3f refract(const Vector3f &I, const Vector3f &N, float refractivity);

unsigned char ReadByte( FILE* file);
void WriteByte( FILE* file, unsigned char b );
unsigned char ClampColorComponent( float c );

#endif //UTILS_H