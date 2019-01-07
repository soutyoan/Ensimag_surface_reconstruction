#ifndef BOX_H
#define BOX_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <omp.h>

#include <Mesh.h>
#include <ImplicitFunction.h>

using namespace std;
using namespace glm;

class Box {

public:
    float x; // x coordinate of the box
    float y; // y coordinate of the box
    float z; // z coordinate of the box

    float lx; // length of the box in x
    float ly; // length of the box in y
    float lz; // length of the box in z

    Box(float _x, float _y, float _z, float _lx, float _ly, float _lz); 


};

#endif
