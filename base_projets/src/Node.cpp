#include "Node.h"

float norm(vec3 x, vec3 y){
	return pow(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2), 0.5);
}

// Function a <Local fit of a general quadric>
// Paper page 4
// float funcToOpt(const std::vector<float> &x, void* data){
// 	/*
// 	Parameters :
// 	A in order
// 		A11 A12 A13 A21 A22 A23 A31 A32 A33
// 	B : 3 coefficients
// 	C : scalar
// 	*/
// 	my_data *d = reinterpret_cast<my_data*>(data);
// 	vector<float> A = d->A;
// 	vector<float> b = d->b;
// 	float c = d->c;
//
// 	return x[0] * (x[0] * A[0] + x[1] * A[1] + x[2] * A[2]) +
// 		   x[1] * (x[0] * A[3] + x[1] * A[4] + x[2] * A[5]) +
// 		   x[2] * (x[0] * A[6] + x[1] * A[7] + x[2] * A[8]) +
// 		   b[0] * x[0] + b[1] * x[1] + b[2] * x[2] + c;
// }
//
// // Action of optimizing the above <funcToOpt> function
// // thanks to the library NLopt
// void optimizeFunction(vec3 vx){
// 	nlopt::opt = opt(nlopt::LN_COBYLA, 2);
// 	opt.set_low_bounds(lb);
// 	opt.set_min_objective(funcToOpt, NULL);
// 	opt.set_xtol_rel(1e-4);
//
// 	vector<float> x(3);
// 	for (int i = 0; i < 3; i++){
// 		x[i] = vx[i];
// 	}
//
// 	double minf;
// 	nlopt::result = opt.optimize(x, minf);
// }

Node::Node(){
	epsi = 0.1;
}

// PARENT CONSTRUCTOR strange before c++11, must do it like this sorry
Node::Node(Box b){
	epsi = 0.001;
	this->b = b;
}

// Return values should be a vector of size 6
// TODO : Find a better and faster way to calculate this.
void Node::getClosestPointsInBall(vector<vec3> &m_vertices, vector<vec3> &m_normals, vector<vec3> &returnValues, vector<vec3> &returnNormals){

	int minimumDistance = 1000000;

	vector<int> return_indices(6);
	bool flag;
	vec3 centerBox = vec3(b.x + b.lx/2, b.y + b.ly/2, b.z + b.lz/2);

	// We are looking for the 6 closest points
	for (int p = 0; p < 6; p++){
		minimumDistance = 1000000;
		for (int i = 0; i < m_vertices.size(); i++){
			flag = false;
			for (int j = 0; j < p; j++){
				if (i == return_indices[j]){
					flag = true;
					break;
				}
			}

			if (flag){
				continue;
			}

			float n = norm(m_vertices[i], centerBox);
			if (n < minimumDistance){
				minimumDistance = n;
				return_indices[p] = i;
			}
		}
		returnValues[p] = m_vertices[return_indices[p]];
		returnNormals[p] = m_normals[return_indices[p]];
	}
}

vec3 Node::getQpoint(int i){
	if (i == 0){
		 return vec3(b.x + b.lx/2, b.y + b.ly/2, b.z + b.lz/2);
	}
	i = i-1;
	return vec3(b.x + (int)(i/4) * b.lx, // 0 0 0 0 1 1 1 1
					b.y + (int)((i%4)/2) * b.ly, // 0 0 1 1 0 0 1 1
					b.z + (i%2) * b.lz); // 0 1 0 1 0 1 0 1
}

vec3 Node::getRemainingQpoints(vector<vec3> &m_vertices, vector<vec3> &m_normals, vector<vec3> &qVec){
	vector<vec3> p(6);
	vector<vec3> n(6);

	getClosestPointsInBall(m_vertices, m_normals, p, n);

	for (int i = 0; i < 9; i++){
		vec3 q = getQpoint(i);
		bool signPositive = dot(n[0], (q - p[0])) > 0;
		bool addQ = true;
		for (int j = 1; j < 6; j++){
			bool currentSign = dot(n[j], (q - p[j])) > 0;
			if (currentSign != signPositive){
				addQ = false;
				break;
			}
		}
		if (addQ){
			qVec.push_back(q);
		}
	}
}

void Node::createQ(){

}

float Node::calculateQi(vec3 x){
	return 0;
}

void Node::setIndices(vector<vec3> &m_vertices, Node& child, vec3 centerNewBox, float radius){
	for (int i = 0; i < indices.size(); i++){
		vec3 currentPoint = m_vertices[indices[i]];
		if (norm(currentPoint, centerNewBox) < radius){
			child.indices.push_back(indices[i]);
		}
	}
}

void Node::createChilds(vector<vec3> &m_vertices){
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
				vec3 centerNewBox(_x + (1 + 2 * x) * b.lx/4, _y + (1 + 2 * y)
					* b.ly/4, _z + (1 + 2 * z) * b.lz/4);
				float radius = sqrt(pow(b.lx/2, 2) + pow(b.ly/2, 2) + pow(b.lz/2, 2));
				setIndices(m_vertices, n, centerNewBox, radius);
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

	// Second formula from the paper
	float num = (Ri - norm(vx, ci));
	num = (num>0) ? num : 0;

	return num/(Ri * norm(vx, ci));
}

// On retourne SwQ et Sw dans le vec2
vec2 Node::MPUapprox(vec3 x, float eps0, vector<vec3> &m_vertices){

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
            createChilds(m_vertices);
        }
        for (int i = 0; i < childs.size(); i++){ // iterere sur les enfants
            vec2 S = childs.at(i).MPUapprox(x, eps0, m_vertices);
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
