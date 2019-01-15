#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <shader.h> // Help to load shaders from files

#include "GLFW_define.h"
#include "Mesh.h"
#include "ImplicitFunction.h"
#include "MeshHE.h"
#include "Object.h"
#include "Mesh_Reconstruction.h"
#include "Implicit_plan_function.h"

#include <sys/stat.h>

int main(){
    // Create a simple cube with a plan
    vector<float> v(8); 
    for (int i = 0; i < 4; i++){
        v[i] = 1; 
    }

    ImplicitPlanFunction i; 
    Box b(0, 0, 0, 1, 1, 1); 
    Mesh m; 
    m.ProcessCube(m, i, 0, b); 

    m.Normalize();
    m.ComputeNormals();
    m.ColorFromNormals();

    // Output to folder
    mkdir("../output", 0777);
    m.write_obj("../output/test_marching_cubes.obj");
}