#include "Node.h"

float norm(vec3 x, vec3 y){
	return pow(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2), 0.5);
}

Node::Node(){
    Q = new vector<float>();
}

// PARENT CONSTRUCTOR strange before c++11, must do it like this sorry
Node::Node(Box b){
	Q = new vector<float>();

	this->b = b;
}

void Node::createQ(){

}

vec2 Node::createChilds(vec3 vx, float eps0){
	// We create subchilds
	// 8 in total, 2 in each direction

	vec2 SGlobal(0, 0);

	for (int x = 0; x < 2; x++){
		for (int y = 0; y < 2; y++){
			for (int z = 0; z < 2; z++){
				float _x = b.x + x/2 * b.lx;
				float _y = b.y + y/2 * b.ly;
				float _z = b.z + z/2 * b.lz;
				Box b(_x, _y, _z, b.lx/2, b.ly/2, b.lz/2);
				Node n(b);
				childs.push_back(n);
				SGlobal += n.MPUapprox(vx, eps0);
			}
		}
	}
}

float Node::calculateWiX(){

}

// On retourne SwQ et Sw dans le vec2
vec2 Node::MPUapprox(vec3 x, float eps0){
    vec2 SGlobal(0, 0);
	if (norm(x, x) > R) {
		return SGlobal;
	}
    if (Q->size() == 0){ // La fonction n'est pas encore créée
        createQ();
    }
    // Le nouveau epsilon a été calculé
    if (epsi > eps0){
        if (childs.size() == 0){
            createChilds(x, eps0);
        }
        for (int i = 0; i < childs.size(); i++){ // iterere sur les enfants
            vec2 S = childs.at(i).MPUapprox(x, eps0);
            SGlobal[0] += S[0];
            SGlobal[1] += S[1];
        }
    } else {
        isLeaf = true;
        SGlobal[0] = 0;
        SGlobal[1] = 0;
    }
    return SGlobal;
}

Node::~Node(){
    delete Q;
}
