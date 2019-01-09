#include "LocalShapeFunction.h"

LocalShapeFunction::LocalShapeFunction() {
    initialized = false;
}

LocalShapeFunction::LocalShapeFunction(vector<float> A, vector<float> B, float C){
    create(A, B, C);
    initialized = false;
}

void LocalShapeFunction::create(vector<float> A, vector<float> B, float C){
    this->A = A;
    this->B = B;
    this->C = C;
    initialized = true;
}

// TODO : Implement
float LocalShapeFunction::calculate(vec3 x){
    if (!isInitialized()){
        throw string("Values not initialized");
    }
    return x[0] * (A[0] * x[0] + A[1] * x[1] + A[2] * x[2]) +
        x[1] * (A[3] * x[0] + A[4] * x[1] + A[5] * x[2]) +
        x[2] * (A[6] * x[0] + A[7] * x[1] + A[8] * x[2]) +
        x[0] * B[0] + x[1] * B[1] + x[2] * B[2] + C;
}
