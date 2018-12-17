#include "Node.h"

float norm(vec3 x, vec3 y){
	return pow(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2), 0.5);
}

Node::Node(){
    Q = new vector<float>();
    childs = new vector<Node>();
}

void Node::createQ(){

}

void Node::createChilds(){

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
        if (childs->size() == 0){
            createChilds();
        }
        for (int i = 0; i < childs->size(); i++){ // iterere sur les enfants
            vec2 S = childs->at(i).MPUapprox(x, eps0);
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
    delete childs;
}
