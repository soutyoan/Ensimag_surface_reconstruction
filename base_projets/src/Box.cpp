#include "Box.h"

Box::Box(float _x, float _y, float _z, float _lx, float _ly, float _lz){
    this->x = _x;
    this->y = _y;
    this->z = _z;
    this->lx = _lx;
    this->ly = _ly;
    this->lz = _lz;
}

Box::Box(){
    x = 0;
    y = 0;
    z = 0;
    lx = 0;
    ly = 0;
    lz = 0;
}

vector<vec3> Box::getListPoints(){
    vector<vec3> points(8);
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 2; j++){
            for (int k = 0; k < 2; k++){
                points[i * 4 + j * 2 + k] = vec3(x + lx * i, y + ly * j, z + lz * k);
            }
        }
    }
    return points; 
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
