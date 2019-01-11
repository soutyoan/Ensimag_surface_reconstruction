/// Implementation of the minimization problem to find the local shape function
#ifndef LOSS_FUNCTION
#define LOSS_FUNCTION

#include "LocalShapeFunction.h"
#include <LBFGS.h>
#include <Eigen/Core>

using namespace std;
using namespace glm;
using namespace LBFGSpp;
using Eigen::VectorXf;


class LossFunction {

public:
    LocalShapeFunction Q; // local shape function Q
    vector<vec3> qVec; // set of auxiliar points
    vector<vec3> pVec;// set of neighbours points p associated to q points
    vector<float> dVec; // set of d values associated with q points
    vector<float> wVec; // set of wi associated to auxiliar points q
    int ITE_MAX = 50; // number of maximal iterations
    float eps=1.0e-4; // precision
    LossFunction(){}
    LossFunction(LocalShapeFunction& Q, const vector<vec3>& _qVec, const vector<vec3>& _pVec, const vector<float>& _dVec, const vector<float>& _wVec);

    float operator()(const VectorXf& X, VectorXf& gradfX);

    VectorXf optimizeQ();

};

#endif
