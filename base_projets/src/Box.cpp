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
