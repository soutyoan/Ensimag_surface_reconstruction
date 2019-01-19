
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <shader.h> // Help to load shaders from files
#include "Node.h"

int main(){
    Node n; 
    vector<vec3> returnValues(6); 
    vector<vec3> returnNormals(6); 
    vector<vec3> m_vertices(60); 
    vector<vec3> m_normals(60);
    vec3 q(1, 1, 1); 
    vector<int> indi(30); 
    for (int i = 0; i < indi.size(); i++){
        indi[i] = i; 
    }
    n.indices = indi; 
    for (int i = 0; i < 60; i++){
        if (i%5 == 0){
            m_vertices[i] = vec3(1.2 + i/60.0, 1.2 + i/60.0, 1.2 + i/60.0); 
            cout << i << " " << 1.2 + i/60.0 << endl; 
        }
    }
    n.getClosestPointsInBall(m_vertices, m_normals, q, returnValues, returnNormals); 

    for (int i = 0; i < returnValues.size(); i++){
        std::cout << returnValues[i][0] << " " << returnValues[i][1] << " " << returnValues[i][2] << " " << endl;
    }
    
}
