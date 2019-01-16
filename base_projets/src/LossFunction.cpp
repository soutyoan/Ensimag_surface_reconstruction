/// Class Loss function implementation

#include "LossFunction.h"

/**
 * Recopied constructor
 * @param _Q    input local shape function object
 * @param _pVec set of p points
 * @param _qVec set of q points
 * @param _dVec set of dq values
 * @param _wVec set of wp values
 */
LossFunction::LossFunction(LocalShapeFunction& _Q, const vector<vec3>& _qVec, const vector<vec3>& _pVec, const vector<float>& _dVec, const vector<float>& _wVec) {
    this->Q = LocalShapeFunction(_Q);
    this->pVec = vector<vec3>(_pVec);
    this->qVec = vector<vec3>(_qVec);
    this->dVec = vector<float>(_dVec);
    this->wVec = vector<float>(_wVec);
    this->W = 0.0;
    this->m = _qVec.size();
    for (int i=0; i<_wVec.size(); i++) {
        this->W += _wVec[i];
    }
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

/**
 * Computation of 13 size vector (x1^2, ..., 1)
 * @param  x input 13 size vector
 * @return   e_x
 */
VectorXf LossFunction::e_x(const vec3& x)
{
    VectorXf res(13);
    res << pow(x[0], 2), x[0]*x[1], x[0]*x[2], x[1]*x[0], pow(x[1], 2), x[1]*x[2], x[2]*x[0], x[2]*x[1], pow(x[2], 2), x[0], x[1], x[2], 1;
    // cout << res <<endl;
    return res;
}

void LossFunction::initM(MatrixXf& M)
{
    MatrixXf M1(13,13);
    MatrixXf M2(13,13);
    for (int p=0; p<pVec.size(); p++) {
        VectorXf ep = e_x(pVec[p]);
        MatrixXf _aux(13,13);
        _aux = ep*ep.transpose();
        M1 += wVec[p]*_aux;
    }

    for (int q=0; q<qVec.size(); q++) {
        VectorXf eq = e_x(qVec[q]);
        M2 += (eq*eq.transpose());
    }

    M = 2.0/W*M1+2.0/m*M2;

    // cout << "M " << endl << M << endl;
}

void LossFunction::initY(VectorXf& y)
{
    if (m==0) {
        return;
    }
    for (int q=0; q<qVec.size(); q++) {
        VectorXf eq = e_x(qVec[q]);
        y += (2.0*dVec[q]/m) * eq;
    }

    // cout << "Y " << y << endl;
}

VectorXf LossFunction::optimizeQ() {

    MatrixXf M(13, 13);
    VectorXf y(13);
    initM(M); initY(y);
    VectorXf x(13);

    x = M.colPivHouseholderQr().solve(y);
    // cout << "solution founded " << x << endl;
    return x;
}
