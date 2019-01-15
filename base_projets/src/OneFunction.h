#include "ImplicitFunction.h"

using namespace glm;
using namespace std;

/*
    Simple Function used to create the normals.
    Will always return a one vector
*/

class OneFunction : public  ImplicitFunction
{
public:

    OneFunction(){}
    ~OneFunction(){}

    virtual float Eval(glm::vec3 p) const;
    virtual glm::vec3 EvalDev(glm::vec3 p) const;


private:

    glm::vec3 m_centre;
    float m_radius;
};
