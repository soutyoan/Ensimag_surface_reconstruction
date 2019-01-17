#include "MPUFunction.h"

MPUFunction::MPUFunction()
{

}

MPUFunction::~MPUFunction()
{

}

float MPUFunction::Eval(glm::vec3 p) const
{
    int p_indice_x = round((p[0] - space.x)*(space.lx/sampling)); 
    int p_indice_y = round((p[1] - space.y)*(space.ly/sampling)); 
    int p_indice_z = round((p[2] - space.z)*(space.lz/sampling)); 

    // We do a 1D interpolation since we are sure that 
    // because of creation, the 2 indices should be on the grid

    return values[(sampling + 5)*(sampling+5) * p_indice_x + (sampling+5) * p_indice_y + p_indice_z]; 
}

glm::vec3 MPUFunction::EvalDev(glm::vec3 p) const
{
    return EvalDevFiniteDiff(p);
}