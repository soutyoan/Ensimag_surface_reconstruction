#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <vector>
#include <string>

#include <AbstractMesh.h>
#include "Node.h"

using namespace std;
using namespace glm;

class ImplicitFunction;

class Mesh : public AbstractMesh
{
public:

    // Constructors
    Mesh(){}                        /// Empty constructor
    Mesh(const char* filename);     /// Imports a mesh from an OFF File

    // utils
    void RemoveDouble(float epsilon = 1e-5);        /// Remove duplicated points


    // primitives generation
    static void CreateCube(Mesh& mesh);             /// Creates a cube (vertices duplicated)
    static void CreateCube2(Mesh& mesh);            /// Creates a cube (vertices not duplicated)
    static void CreateSphere(Mesh& mesh, unsigned int Nu = 100, unsigned int Nv = 50);  /// Creates a UV sphere



    // marching tetrahedra
    static void CreateIsoSurface(Mesh& mesh, const ImplicitFunction& function, const float isoValue = 0.5
            , const float minX = -1.0, const float maxX = 1.0
            , const float minY = -1.0, const float maxY = 1.0
            , const float minZ = -1.0, const float maxZ = 1.0
            , const unsigned int resX = 100
            , const unsigned int resY = 100
            , const unsigned int resZ = 100);           /// Implements the marching cube algorithm

    static void ProcessCube(Mesh& mesh, const ImplicitFunction& function, const float isoValue, const vector<vec3> p);

    /// Processes a tetrahedron during marching tetrahedron algorithm
    static void ProcessTetrahedron(Mesh& mesh, const ImplicitFunction& function, const float isoValue, const glm::vec3 p[]);

    Node root;
    float evaluateMPUapprox(Mesh& mesh, glm::vec3 x, float eps0);

    void clearIndicesAndVertices();

    /*
    Recursive call on the marching cubes.
    */
    static void MarchingCubes(Mesh &m, Node &current);

    int getVerticesSize(){return m_positions.size(); }
    int getIndicesSize(){return m_indices.size(); }
    int getNormalsSize(){return m_normals.size(); }
    vector<vec3> getNormals(){return m_normals; }
};

#endif // MESH_H
