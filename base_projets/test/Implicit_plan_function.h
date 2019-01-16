#include "ImplicitFunction.h"

class ImplicitPlanFunction : public  ImplicitFunction
{
public:

    ImplicitPlanFunction(){}
    ~ImplicitPlanFunction(){}

    virtual float Eval(glm::vec3 p) const;
    virtual glm::vec3 EvalDev(glm::vec3 p) const;


private:

    glm::vec3 m_centre;
    float m_radius;
};