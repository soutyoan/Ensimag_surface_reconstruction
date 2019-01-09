#ifndef NODE_H
#define NODE_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <omp.h>

#include <ImplicitFunction.h>
#include "Box.h"

// Optimisation library get from https://nlopt.readthedocs.io/en/latest/#documentation-and-mailing-lists
// #include <nlopt.h>

using namespace std;
using namespace glm;

typedef struct {
    vector<float> A;
    vector<float> b;
    float c;
} my_data;

class Node{

public:
    float epsi; // Epsilon i
    bool isRoot; 
    vector<float> Q; // La fonction d'approximation Q
    vector<Node> childs; // Childs of this node
    bool isLeaf;
    vector<int> indices;
    Box b;

    Node();
    Node(Box b);

    // Functions from the MPU_omplicit paper, page 3
    vec2 MPUapprox(vec3 x, float eps0, vector<vec3> &vertices, vector<vec3> &normals);

    void createQ(vector<vec3> &m_vertices, vector<vec3> &m_normals);
    void createChilds(vector<vec3> &m);
    float calculateWiX(vec3 vx);
    float calculateQi(vec3 x);
    void setIndices(vector<vec3> &m, Node& child, vec3 centerNewBox, float radius);
    void getClosestPointsInBall(vector<vec3> &m_vertices, vector<vec3> &m_normals, vector<vec3> &returnValues, vector<vec3> &returnNormals);
    vec3 getQpoint(int i);
    vec3 getRemainingQpoints(vector<vec3> &m_vertices, vector<vec3> &m_normals, vector<vec3> &qVec);
    ~Node();
};

#endif
