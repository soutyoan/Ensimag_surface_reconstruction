/// Class Loss function implementation

#include "LossFunction.h"

/**
 * Recopied constructor
 * @param _Q    input local shape function object
 * @param _pVec set of p points
 * @param _qVec [description]
 * @param _dVec [description]
 * @param _wVec [description]
 */
LossFunction::LossFunction(LocalShapeFunction& _Q, const vector<vec3>& _qVec, const vector<vec3>& _pVec, const vector<float>& _dVec, const vector<float>& _wVec) {
    this->Q = LocalShapeFunction(_Q);
    this->pVec = vector<vec3>(_pVec);
    this->qVec = vector<vec3>(_qVec);
    this->dVec = vector<float>(_dVec);
    this->wVec = vector<float>(_wVec);
}


float LossFunction::operator()(const VectorXf& X, VectorXf& gradfX) {
    vector<vec3>::iterator q;
    vector<vec3>::iterator p;
    vector<float>::iterator d;
    vector<float>::iterator w;

    float res;

    Q.updateQ(X);

    float W = 0;
    for (w=wVec.begin(); w!=wVec.end(); w++) {
        W += *w;
    }

    if (W == 0){
        W = 1;
    }

    int m=qVec.size();

    for (int i=0; i<13; i++) {

    	float val1 = 0;
        float val2 = 0;
    	w = wVec.begin();

    	for (p=pVec.begin(); p!=pVec.end(); p++) {
    		float _tmp = (*w * Q.calculate(*p)) * 2;
            res += *w * pow(Q.calculate(*p), 2) / W;
    		w++;
    		// 9 components for matrix A
    		if (i<9) {
    			_tmp *= (*p)[int(i/3)] * (*p)[int(i%3)];
    		} else if (i<12) { // 3 components for vector B
    			_tmp *= (*p)[i-9];
    		}
    		val1 += _tmp; // last one for value C
    	}

    	for (q=qVec.begin(), d=dVec.begin(); q!=qVec.end() || d!=dVec.end(); q++, d++) {
            // cout << (*d) << " " << (*q)[0] << " " << dVec.size() << endl;
    		float _tmp = (this->Q.calculate(*q) - *d) * 2;
            res += pow(Q.calculate(*q) - *d, 2) / m;
    		// 9 components for matrix A
    		if (i<9) {
    			_tmp *= (*q)[int(i/3)] * (*q)[int(i%3)];
    		} else if (i<12) { // 3 components for vector B
    			_tmp *= (*q)[i-9];
    		}
    		val2 += _tmp; // last one for value C
    	}

    	gradfX[i] = val1 / W + val2 / m;
        // cout << X[i] << " " << val1 << " " << val2 << " " << m << " " << W << endl;
 	}

    return res;
}

VectorXf LossFunction::optimizeQ() {
    LBFGSParam<float> param;
    param.epsilon = eps;
    param.max_iterations = ITE_MAX;

    LBFGSSolver<float> solver(param);

    VectorXf X(13);
    for (int i = 0; i< 13; i++){
        X[i] = i;
    }

    float _val;
    int niter = solver.minimize(*this, X, _val);
    // cerr << "iter " << niter << endl;

    return X;
}
