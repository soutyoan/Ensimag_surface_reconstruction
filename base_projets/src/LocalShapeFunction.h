#ifndef LOCAL_SHAPE_FUNCTION_H
#define LOCAL_SHAPE_FUNCTION_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <omp.h>

using namespace std;
using namespace glm;

class LocalShapeFunction {

public:

    /*0 : A11
    1 : A12
    2 : A13
    3 : A21
    4 : A22
    5 : A23
    6 : A31
    7 : A32
    8 : A33
    */
    vector<float> A;

    /*
    9 : B1
    10 : B2
    11 : B3
    */
    vector<float> B;

    float C;

    LocalShapeFunction();
    LocalShapeFunction(vector<float> A, vector<float> B, float C);
    vec3 calculate(vec3 x);
    bool isInitialized(); 
};

#endif
