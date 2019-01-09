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

void LocalShapeFunction::GetOneZero(float x, float y, vector<vec3>& solutions){
    // Find these coefficients by developing the Q function
    float a = A[8];
    float b = x * (A[6] + A[2]) + y * (A[5] + A[7]) + B[2];
    float c = pow(x, 2) * A[0] + (A[1] + A[3]) * x * y +
            pow(y, 2) * A[4] + B[0] * x + B[1] * y + C;

    float delta = pow(b, 2) - 4 * c * a;
    if (delta < 0){
        // No solution
    } else if (delta == 0){
        vec3 sol = vec3(x, y, -b/(2 * a));
        solutions.push_back(sol);
    } else {
        vec3 sol1 = vec3(x, y, (-b + sqrt(delta))/(2 * a));
        vec3 sol2 = vec3(x, y, (-b - sqrt(delta))/(2 * a));
        solutions.push_back(sol1);
        solutions.push_back(sol2);
    }
}

void LocalShapeFunction::FindZeros(Box& b, vector<vec3>& returnValues){
    // We solve the equation by looking for z,
    // with a fixed x and y
    // This gives a quadratic equation.
    if (!isInitialized()){
        throw string("Values not initialized");
    }


    // TODO :  For now we are only using the boundaries of the cube
    // as our sampling bounds but we could create points outside the points actually existing.
    // We should find a good method finding biggest and lowest points in x
    // and y directions to sample from them

    for (int x = b.x; x <= b.x + b.lx; x+= b.lx/SAMPLE_RATE){
        for (int y = b.y; x <= b.y + b.ly; y+= b.ly/SAMPLE_RATE){
            vector<vec3> solutions;
            GetOneZero(x, y, solutions);
            for (int i = 0; i < solutions.size(); i++){
                returnValues.push_back(solutions[i]);
            }
        }
    }

    // Value returned in returnValues
}
