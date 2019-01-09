#ifndef BOX_H
#define BOX_H

#include <ostream>

using namespace std;

class Box {

public:
    float x; // x coordinate of the box
    float y; // y coordinate of the box
    float z; // z coordinate of the box

    float lx; // length of the box in x
    float ly; // length of the box in y
    float lz; // length of the box in z

    Box(float _x, float _y, float _z, float _lx, float _ly, float _lz);
    Box();

};

std::ostream& operator<<(std::ostream &strm, const Box &b); 

#endif
