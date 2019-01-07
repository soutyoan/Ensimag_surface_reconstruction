#include "Node.h"

float norm(vec3 x, vec3 y){
	return pow(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2), 0.5);
}

// Function a <Local fit of a general quadric>
// Paper page 4
float funcToOpt(const std::vector<float> &x, void* data){
	/*
	Parameters :
	A in order
		A11 A12 A13 A21 A22 A23 A31 A32 A33
	B : 3 coefficients
	C : scalar
	*/
	my_data *d = reinterpret_cast<my_data*>(data);
	vector<float> A = d->A;
	vector<float> b = d->b;
	float c = d->c;

	return x[0] * (x[0] * A[0] + x[1] * A[1] + x[2] * A[2]) +
		   x[1] * (x[0] * A[3] + x[1] * A[4] + x[2] * A[5]) +
		   x[2] * (x[0] * A[6] + x[1] * A[7] + x[2] * A[8]) +
		   b[0] * x[0] + b[1] * x[1] + b[2] * x[2] + c;
}

// Action of optimizing the above <funcToOpt> function
// thanks to the library NLopt
void optimizeFunction(vec3 vx){
	nlopt::opt = opt(nlopt::LN_COBYLA, 2);
	opt.set_low_bounds(lb);
	opt.set_min_objective(funcToOpt, NULL);
	opt.set_xtol_rel(1e-4);

	vector<float> x(3);
	for (int i = 0; i < 3; i++){
		x[i] = vx[i];
	}

	double minf;
	nlopt::result = opt.optimize(x, minf);
}

Node::Node(){
	epsi = 0.1;
}

// PARENT CONSTRUCTOR strange before c++11, must do it like this sorry
Node::Node(Box b){
	epsi = 0.001;
	this->b = b;
}

void Node::createQ(){

}

float Node::calculateQi(vec3 x){
	// TODO: calculate result
	return 0;
}

void Node::createChilds(){
	// We create subchilds
	// 8 in total, 2 in each direction

	for (int x = 0; x < 2; x++){
		for (int y = 0; y < 2; y++){
			for (int z = 0; z < 2; z++){
				float _x = b.x + x/2 * b.lx;
				float _y = b.y + y/2 * b.ly;
				float _z = b.z + z/2 * b.lz;
				Box b(_x, _y, _z, b.lx/2, b.ly/2, b.lz/2);
				Node n(b);
				childs.push_back(n);
			}
		}
	}
}

float Node::calculateWiX(vec3 vx){
	// Ci is the center of the box
	vec3 ci(b.x + b.lx/2, b.y + b.ly/2, b.z + b.lz/2);
	// Radius of the cube
	float Ri = sqrt(pow(b.lx/2, 2) + pow(b.ly/2, 2) + pow(b.lz/2, 2));

	float x = 3 * norm(ci, vx)/Ri;

	// TODO : Apply b spline to x

	return 0;
}

// On retourne SwQ et Sw dans le vec2
vec2 Node::MPUapprox(vec3 x, float eps0){

	cout << "HELLO\n" ;

    vec2 SGlobal(0, 0);
	if (norm(x, x) > R) {
		return SGlobal;
	}
    if (Q.size() == 0){ // La fonction n'est pas encore créée
        createQ();
    }
    // Le nouveau epsilon a été calculé
    if (epsi > eps0){
        if (childs.size() == 0){
            createChilds();
        }
        for (int i = 0; i < childs.size(); i++){ // iterere sur les enfants
            vec2 S = childs.at(i).MPUapprox(x, eps0);
            SGlobal[0] += S[0];
            SGlobal[1] += S[1];
        }
    } else {
        isLeaf = true;
		float wix = this->calculateWiX(x);
        SGlobal[0] += wix * calculateQi(x);
        SGlobal[1] += wix;
    }
    return SGlobal;
}

Node::~Node(){

}
