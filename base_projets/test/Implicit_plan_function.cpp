#include "Implicit_plan_function.h"

float SphereFunction::Eval(glm::vec3 p) const
{
    return p[0] - 0.5;
}

glm::vec3 SphereFunction::EvalDev(glm::vec3 p) const
{
    return glm::vec3(1, 0, 0);
}