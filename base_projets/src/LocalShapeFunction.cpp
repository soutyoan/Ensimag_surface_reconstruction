#include "LocalShapeFunction.h"

LocalShapeFunction::LocalShapeFunction() {

}

LocalShapeFunction::LocalShapeFunction(vector<float> A, vector<float> B, float C){
    this->A = A;
    this->B = B;
    this->C = C;
}

// TODO : Implement
vec3 LocalShapeFunction::calculate(vec3 x){
    return vec3(0);
}

bool LocalShapeFunction::isInitialized(){
    return false; 
}
