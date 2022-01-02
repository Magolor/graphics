
#include "utils.hpp"
#include <iostream>

Vector3f reflect(const Vector3f &I, const Vector3f &N){return I+Vector3f::dot(-I,2.*N)*N;}
Vector3f refract(const Vector3f &I, const Vector3f &N, float refractivity){
    float cosd = Vector3f::dot(I,N), sin2d = 1-sqr(cosd), eta, ratio;
    Vector3f Nd = cosd<0?cosd=-cosd,eta=refractivity,-N:(eta=1/refractivity,N);
    return (ratio=sqr(eta)*sin2d)<=1.?I*eta+Nd*(eta*cosd-sqrt(1-ratio)):Vector3f::ZERO;
}

unsigned char ReadByte( FILE* file) {
    unsigned char b;
    int success = fread( ( void* )( &b ), sizeof( unsigned char ), 1, file );
    assert( success == 1 );
    return b;
}

void WriteByte( FILE* file, unsigned char b ) {
    int success = fwrite( ( void* )( &b ), sizeof( unsigned char ), 1, file );
    assert( success == 1 );
}

unsigned char ClampColorComponent( float c ) {
    int tmp = int( c * 255 ); return ( unsigned char ) (tmp>=0 ? tmp<=255 ? tmp : 255 : 0);
}