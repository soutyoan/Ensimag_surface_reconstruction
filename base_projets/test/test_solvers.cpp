/// Comparaison bet. gradient descent and linear solver
#include "LossFunction.h"

int main()
{
    vector<vec3> pVec;
    pVec.push_back(vec3(1, 0, -653));
    pVec.push_back(vec3(0, 1, 0));
    pVec.push_back(vec3(0, 0, 1));
    vector<vec3> qVec = vector<vec3>(pVec);
    vector<float> dVec;
    dVec.push_back(1.0);
    dVec.push_back(1.0);
    dVec.push_back(1.0);
    vector<float> wVec = vector<float>(dVec);
    LocalShapeFunction Q;
    LossFunction g(Q, pVec, qVec, dVec, wVec);
    VectorXf sol = g.optimizeQ();
    Q.updateQ(sol);
    cout << "value " << Q.Eval(vec3(1, 0, 0))<<endl;
    cout << "value " << Q.Eval(vec3(0, 1, 0))<<endl;
    cout << "value " << Q.Eval(vec3(0, 0, 1))<<endl;
    cout << "value " << Q.Eval(vec3(1, 1, 1))<<endl;
    return EXIT_SUCCESS;
}
