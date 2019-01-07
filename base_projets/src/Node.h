#ifndef NODE_H
#define NODE_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <omp.h>

#include <Mesh.h>
#include <ImplicitFunction.h>
#include "Box.h"

using namespace std;
using namespace glm;

class Node{

public:
    float epsi; // Epsilon i
    vector<float>* Q; // La fonction d'approximation Q
    vector<Node> childs; // Childs of this node
    bool isLeaf;
    Box b;

    Node();
    Node(Box b);
    vec2 MPUapprox(vec3 x, float eps0);
    void createQ();
    vec2 createChilds(vec3 x, float eps0);
    float calculateWiX();
    ~Node();
};


#endif
