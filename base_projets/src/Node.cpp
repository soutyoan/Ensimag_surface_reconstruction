#include "Node.h"

float norm(vec3 x, vec3 y){
	return pow(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2), 0.5);
}

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
void Node::getClosestPointsInBall(vector<vec3> &m_vertices, vector<vec3> &m_normals, vec3 q, vector<vec3> &returnValues, vector<vec3> &returnNormals){

	int minimumDistance = 1000000;

	vector<int> return_indices(6);
	bool flag;

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

	for (int i = 0; i < 9; i++){
		vec3 q = getQpoint(i);
		getClosestPointsInBall(m_vertices, m_normals, q, p, n);
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

void Node::createQ(vector<vec3> &m_vertices, vector<vec3> &m_normals){

}

float Node::calculateQ(vec3 x){
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
vec2 Node::MPUapprox(vec3 x, float eps0, vector<vec3> &m_vertices, vector<vec3> &m_normals){

	cout << "HELLO\n" ;

    vec2 SGlobal(0, 0);
	if (norm(x, x) > R) {
		return SGlobal;
	}
    if (!Q.isInitialized()){ // La fonction n'est pas encore créée
        createQ(m_vertices, m_normals);
    }
    // Le nouveau epsilon a été calculé
    if (epsi > eps0){
        if (childs.size() == 0){
            createChilds(m_vertices);
        }
        for (int i = 0; i < childs.size(); i++){ // iterere sur les enfants
            vec2 S = childs.at(i).MPUapprox(x, eps0, m_vertices, m_normals);
            SGlobal[0] += S[0];
            SGlobal[1] += S[1];
        }
    } else {
        isLeaf = true;
		float wix = this->calculateWiX(x);
        SGlobal[0] += wix * calculateQ(x);
        SGlobal[1] += wix;
    }
    return SGlobal;
}

void Node::initializeAsRoot(int sizeVertices){
	indices.resize(sizeVertices);
	for (int i = 0; i < sizeVertices; i++){
		indices[i] = i;
	}
}

Node::~Node(){

}
