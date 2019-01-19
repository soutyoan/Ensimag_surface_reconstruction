#include "ImplicitFunction.h"
#include "Box.h"

using namespace glm;
using namespace std;

class MPUFunction : public ImplicitFunction
{
public:

    int sampling; 
    Box space; 
    vector<float> values; 

    MPUFunction(); 
    ~MPUFunction();

    virtual float Eval(glm::vec3 p) const;
    vec3 EvalDev(glm::vec3 p) const; 
};
