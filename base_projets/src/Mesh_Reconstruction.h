#ifndef MESH_RECONSTRUCTION_H
#define MESH_RECONSTRUCTION_H

#include "Mesh.h"
#include "OneFunction.h"

class Mesh_Reconstruction : public Mesh {

public:
    Mesh_Reconstruction() : Mesh(){};
    Mesh_Reconstruction(const char* filename) : Mesh(filename){}

    static void ProcessCube(Mesh& mesh, const vector<vec3> gradients, vector<float> MPU_Values, const vector<vec3> points);

    /// Processes a tetrahedron during marching tetrahedron algorithm
    static void ProcessTetrahedron(Mesh& mesh, const vector<vec3> gradients, const float MPU_values[], const glm::vec3 p[]);

    Node root;
    float evaluateMPUapprox(Mesh& mesh, glm::vec3 x, float eps0, Box broot);

    void clearIndicesAndVertices();

    void GetVertices(int sampling, Mesh_Reconstruction &m, float eps0);

    /*
    Recursive call on the marching cubes.
    */
    // static void MarchingCubes(Mesh &m, Node &current);

    int getVerticesSize(){return m_positions.size(); }
    int getIndicesSize(){return m_indices.size(); }
    int getNormalsSize(){return m_normals.size(); }
    vector<vec3> getNormals(){return m_normals; }

};

#endif
