#ifndef LOCAL_SHAPE_FUNCTION_H
#define LOCAL_SHAPE_FUNCTION_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <stdexcept>
#include "Box.h"

using namespace std;
using namespace glm;

const int SAMPLE_RATE = 5;

// Q function x^TAx + b^Tx + c
class LocalShapeFunction {

private:
    bool initialized;

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
    LocalShapeFunction(vector<float>& A, vector<float>& B, float C);
    LocalShapeFunction(vector<float>& X);
    LocalShapeFunction(const LocalShapeFunction& _aux);

    float calculate(vec3 x);


    bool isInitialized(){return initialized;}

    /*
    Initialize the function
    */
    void create(vector<float> A, vector<float> B, float C);

    /*
    Updating the function
     */
    void updateQ(vector<float> X);

    /*
    Find the zeros of the function.
    Iterate over the points to sample them and get all the zeros.
    */
    void FindZeros(Box& b, vector<vec3>& returnValues);

    /*
    Get one zero of the function.
    */
    void GetOneZero(float x, float y, vector<vec3>& solutions);
};

#endif
