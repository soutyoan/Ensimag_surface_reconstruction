#include "Node.h"

float norm(vec3 x, vec3 y){
	return sqrt(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2));
}

/**
 * Computation of L2 norm for float vector (used in optimization algorithm)
 * @param  vec vector of float values
 * @return     ||vec|| (l2-norm)
 */
float normVec(vector<float> vec) {
	vector<float>::iterator val;
	float res;
	for (val=vec.begin(); val!=vec.end(); val++) {
		res += pow(*val, 2);
	}
	return sqrt(res);
}

Node::Node(){
	isLeaf = false;
	epsi = 0;
}

// PARENT CONSTRUCTOR strange before c++11, must do it like this sorry
Node::Node(Box b){
	epsi = 0;
	isLeaf = false;
	this->b = b;
}

Node::Node(const Node& n){
	this->epsi = n.epsi;
	this->isLeaf = false;
	this->childs = vector<Node>(n.childs);
	this->indices = vector<int>(n.indices);
	this->b = n.b;
}

// Return values should be a vector of size 6
// TODO : Find a better and faster way to calculate this.
void Node::getClosestPointsInBall(vector<vec3> &m_vertices, vector<vec3> &m_normals, vec3 q, vector<vec3> &returnValues, vector<vec3> &returnNormals){

	int minimumDistance = 1000000;

	vector<int> return_indices(6);
	bool flag;

	if (indices.size() == 0){
		isLeaf = true;
		return;
	}

	// We need at least 6 points in the list of indices.
	// We add them all in the p list
	if (indices.size() < 6){
		for (int p = 0; p < indices.size(); p++){
			returnValues[p] = m_vertices[indices[p]];
			returnNormals[p] = m_normals[indices[p]];
		}
		for (int p = indices.size(); p < 6; p++){
			// We set the same point to complete the vector
			// This will have no impact on the program,
			// we just want to avoid a non initialized value.
			returnValues[p] = m_vertices[indices[0]];
			returnNormals[p] = m_normals[indices[0]];
		}
		return;
	}

	// We are looking for the 6 closest points
	for (int p = 0; p < 6; p++){
		minimumDistance = 1000000;
		// We only need to check in the current ball indices,
		// these indices were saved during the cretae child function
		for (vector<int>::iterator it = indices.begin(); it != indices.end(); ++it){
			int i = *it; // Getting the ienteger from the iterator
			flag = false;
			for (int j = 0; j < p; j++){
				if (i == return_indices[j]){
					flag = true;
					break;
				}
			}

			if (flag){
				// If the current index is already in the list we
				// don't put it in the list again.
				continue;
			}

			float n = norm(m_vertices[i], q);
			if (n < minimumDistance){
				minimumDistance = n;
				return_indices[p] = i;
			}
		}
		returnValues[p] = m_vertices[return_indices[p]];
		returnNormals[p] = m_normals[return_indices[p]];
		// cout << "return index " << m_normals[return_indices[p]][0] << " index " << return_indices[p] << endl;
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

/**
 * Gets the remaining points in order to compute the local shape function Q in
 * general form (a).
 * @param  m_vertices input points vertex
 * @param  m_normals  input normal vertex
 * @param  qVec       remaining auxiliar points q
 * @param  pVec       set of p points associated to each q point
 * @param dVec	      set of d values
 * @return            updating qvec and pVec sets
 */
vec3 Node::getRemainingQpoints(vector<vec3> &m_vertices, vector<vec3> &m_normals, vector<vec3> &qVec, vector <vec3> &pVec, vector <float> &dVec){
	vector<vec3> p(6);
	vector<vec3> n(6);

	for (int i = 0; i < 9; i++){
		vec3 q = getQpoint(i);
		getClosestPointsInBall(m_vertices, m_normals, q, p, n);
		if (isLeaf){return vec3(0); }
		bool signPositive = dot(n[0], (q - p[0])) > 0;
		bool addQ = true;
		for (int j = 0; j < 6; j++){
			bool currentSign = dot(n[j], (q - p[j])) > 0;
			if (currentSign != signPositive){
				addQ = false;
				break;
			}
		}
		if (addQ){
			qVec.push_back(q);
			float d = 0;
			for (int _ind=0; _ind<6; _ind++) {
				// cout << "d " << d << " " << i << " " << n[_ind][0] << " " << n[_ind][1] << " " << n[_ind][2] << "\n";
				pVec.push_back(vec3(p[_ind]));
				d += dot(n[_ind], q-p[_ind]);
			}
			d /= 6;
			// cout << "d " << d << "\n";
			dVec.push_back(d);
		}
	}
}



void Node::createQ(vector<vec3> &m_vertices, vector<vec3> &m_normals){
	vector<vec3> qVec;
	vector<vec3> pVec;
	vector<float> dVec;
	getRemainingQpoints(m_vertices, m_normals, qVec, pVec, dVec);
	if (isLeaf){return; }
	vector<float> wVec(pVec.size());
	for (int i=0; i<pVec.size(); i++) {
		wVec[i] = calculateWiX(pVec[i]);
	}
	LossFunction F(this->Q, qVec, pVec, dVec, wVec);

	VectorXf X = F.optimizeQ();

	Q.updateQ(X);

}

float Node::calculateQ(vec3 x){
	return Q.calculate(x);
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
				float _x = b.x + x/2.0 * b.lx;
				float _y = b.y + y/2.0 * b.ly;
				float _z = b.z + z/2.0 * b.lz;
				Box new_b(_x, _y, _z, b.lx/2, b.ly/2, b.lz/2);
				Node n(new_b);
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

	// Second formula from the paper
	float num = (Ri - norm(vx, ci));
	// cout << "num " << num << " " << Ri << " " << norm(vx, ci) << endl;
	num = (num>0) ? num : 0;

	return pow(num/(Ri * norm(vx, ci)), 2);
}

vec3 Node::gradWi(vec3 vx) {
	vec3 res(0);
	// TODO
	return res;
}

// On retourne SwQ et Sw dans le vec2
/**
 * Computation of f(x) approximation + gradient(f(x))
 * @param  x             input 3D point
 * @param  eps0          error rate
 * @param  m_vertices    set of given points
 * @param  m_normals     set of given normals
 * @param  MPUapproxGrad output normal vector grad(f(x))
 * @return               SwQ, Sw in order to get f(x) = SwQ / Sw
 */
vec2 Node::MPUapprox(vec3 x, float eps0, vector<vec3> &m_vertices, vector<vec3> &m_normals){

    vec2 SGlobal(0, 0);
	float Ri = sqrt(pow(b.lx/2, 2) + pow(b.ly/2, 2) + pow(b.lz/2, 2));
	vec3 ci(b.x + b.lx/2, b.y + b.ly/2, b.z + b.lz/2);
	if (norm(x, ci) > Ri) {
		return SGlobal;
	}
    if (!Q.isInitialized()){ // La fonction n'est pas encore créée
        createQ(m_vertices, m_normals);
		if (isLeaf){
			return vec2(0, 0);
		}
		epsi = 0;
		for (int i = 0; i < indices.size(); i++){
			float current = abs(Q.calculate(m_vertices[indices[i]])/norm(Q.evalGradient(m_vertices[indices[i]]), vec3(0)));
			epsi = (current > epsi) ? current : epsi;
		}

    }
	// cout << "epsi " << epsi << endl;
    // Le nouveau epsilon a été calculé
    if (epsi > eps0){
        if (childs.size() == 0){
            createChilds(m_vertices);
        }
        for (int i = 0; i < childs.size(); i++){ // iterere sur les enfants
			// cout << "CREATE CHILDS\n";
            vec2 S = childs.at(i).MPUapprox(x, eps0, m_vertices, m_normals);
            SGlobal[0] += S[0];
            SGlobal[1] += S[1];
			// cout << "global " << SGlobal[0] << " " << SGlobal[1] << endl;
        }
    } else {
        isLeaf = true;
		float wix = this->calculateWiX(x);
		// cerr << "WIX  XXXXXXXXXXXXXXXXXXXXX" << wix << endl;
		vec3 ci(b.x + b.lx/2, b.y + b.ly/2, b.z + b.lz/2);
		// cerr << Ri << " " << norm(x, ci) << endl;
        SGlobal[0] += wix * calculateQ(x);
        SGlobal[1] += wix;
		// cout << "ici " << SGlobal[0] << " " << SGlobal[1] << endl;
    }
    return SGlobal;
}

void Node::initializeAsRoot(int sizeVertices){
	indices.resize(sizeVertices);
	for (int i = 0; i < sizeVertices; i++){
		indices[i] = i;
	}
}

void Node::getAllBoxes(vector<Box> &boxes){
	for (int i = 0; i < childs.size(); i++){
		Node n = childs[i];
		if (childs[i].isLeaf){
			boxes.push_back(childs[i].b);
		} else {
			n.getAllBoxes(boxes);
		}
	}
}

Node::~Node(){

}
