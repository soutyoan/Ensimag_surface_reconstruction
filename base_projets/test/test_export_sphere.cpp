
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

#include <sys/stat.h>

// Window size :
#define WIDTH 1000.0f
#define HEIGHT 800.0f

using namespace glm;
using namespace std;

#define DEBUG false

const bool OUTPUT = true;

int main()
{

    cout << "Starting program..." << endl;


    //-------------------------------------------------
    // Data arrays Initialization

    // Mesh creation
    // Mesh m("../models/triceratops.off");

    BarthFunction bf;
    float x = 1.8;
    Mesh_Reconstruction m;
    m.CreateSphere(m, 500, 500);

    // m.RemoveDouble(0.5);
    m.Normalize();
    m.ComputeNormals();
    m.ColorFromNormals();

    if (OUTPUT){
        // Output to folder
        mkdir("../output", 0777);
        m.write_obj("../output/sphere.obj");
    }

    // Half edge conversion
    MeshHE m_he(m);

    cout << endl << endl << "Main loop ended." << endl;


    cout << "Program ended." << endl;


    return EXIT_SUCCESS;
}