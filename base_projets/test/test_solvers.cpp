/// Comparaison bet. gradient descent and linear solver
#include "LossFunction.h"

int main()
{
    vector<vec3> pVec;
    vector<vec3> qVec;
    vector<float> dVec;
    vector<float> wVec;
    for (int i=0; i<3; i++) {
        vec3 p = vec3(rand()%10, rand()%10, rand()%10);
        vec3 q = vec3(rand()%10, rand()%10, rand()%10);
        pVec.push_back(p);
        qVec.push_back(q);
        dVec.push_back(rand()%5);
        wVec.push_back(rand()%5);
    }
    LocalShapeFunction Q;
    LossFunction g(Q, pVec, qVec, dVec, wVec);
    VectorXf sol = g.optimizeQ();
    Q.updateQ(sol);
    cout << rand()%2 << endl;
    return EXIT_SUCCESS;
}
