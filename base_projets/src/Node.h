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
#include "LocalShapeFunction.h"
#include "LossFunction.h"

#include <Eigen/Core>

// Optimisation library get from https://nlopt.readthedocs.io/en/latest/#documentation-and-mailing-lists

using namespace std;
using namespace glm;
using Eigen::VectorXf;

class Node{

private:
    float epsi; // Epsilon i
    vector<Node> childs; // Childs of this node
    bool isLeaf;
    vector<int> indices;

public:

    LocalShapeFunction Q;

    // The box including all the points
    // This box is created by a subdivision
    Box b;

    /*
    Constructors
    */
    Node();
    Node(Box b);
    Node(const Node& n);

    /*
    Function from the MPU_omplicit paper, page 3
    */
    vec2 MPUapprox(vec3 x, float eps0, vector<vec3> &vertices, vector<vec3> &normals);

    /*
    Create the Q function that will be used to recreate the mesh.
    We use the (a) function from page 4 in the paper.
    With the form Q = x^TAx + b^Tx + c
    With A size (3, 3)
    b size (3, 1)
    and c a scalar
    */
    void createQ(vector<vec3> &m_vertices, vector<vec3> &m_normals);

    /*
    Box subdivision to part the space in 8 new boxes
    (2 in each direction)
    */
    void createChilds(vector<vec3> &m);

    /*
    Calculate the Wi(x) coefficient.
    See (3), page 3 of the paper
    */
    float calculateWiX(vec3 vx);

    /*
    Calculate the result of the Q function at the point x
    */
    float calculateQ(vec3 x);

    /*
    Set the indices refering to the vertices from the mesh
    during the division of the space in boxes.
    */
    void setIndices(vector<vec3> &m, Node& child, vec3 centerNewBox, float radius);

    /*
    Returns the 6 closest points in the ball compared to q.
    */
    void getClosestPointsInBall(vector<vec3> &m_vertices, vector<vec3> &m_normals, vec3 q, vector<vec3> &returnValues, vector<vec3> &returnNormals);

    /*
    For i in [0, 8], returns the i-th point of the q vector.
    The points of Q are the corner of the box and the center
    */
    vec3 getQpoint(int i);

    /*
    Calculate the remaining Q points, used for the creation of the Q function.
    See the declaration of the Q function on the page 4 (a) of the paper
    */
    vec3 getRemainingQpoints(vector<vec3> &m_vertices, vector<vec3> &m_normals,
        vector<vec3> &qVec, vector <vec3> &pVec, vector <float> &dVec);

    /*
    Compute the gradients and returns the result in the res parameter.
    This res parameter should be initialize as a vector<float>(13)
    */
    void compute_gradient(vector<vec3> &qvector, vector<vec3> &pvector,
         vector <float> &dVec, vector<float>& res);

    /*
    We initialize the indices vector for the root,
    with the indices being all the indices from the vertices.
    The indices vector from the child will be initialized during the
    child creation, thanks to the setIndices function.
    */
    void initializeAsRoot(int sizeVertices);

    /*
    Destructor
    */
    ~Node();
};

#endif
