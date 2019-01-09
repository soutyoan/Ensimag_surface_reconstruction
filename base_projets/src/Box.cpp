#include "Box.h"

Box::Box(float _x, float _y, float _z, float _lx, float _ly, float _lz){
    x = _x;
    y = _y;
    z = _z;
    lx = _lx;
    ly = _ly;
    lz = _lz;
}

Box::Box(){
    x = 0;
    y = 0;
    z = 0;
    lx = 0;
    ly = 0;
    lz = 0;
}

std::ostream& operator<<(std::ostream &strm, const Box &b) {
    strm << "Box : \n"
        << "x " << b.x << "\n"
        << "y " << b.y << "\n"
        << "z " << b.z << "\n"
        << "lx " << b.lx << "\n"
        << "ly " << b.ly << "\n"
        << "lz " << b.lz << "\n";
    return strm;
}
